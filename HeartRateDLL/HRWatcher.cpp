#include "pch.h"
#include "HRWatcher.h"
#include "HeartRateMonitor.h"

// ������� ���������� ����������� �������
#define GATT_AWAIT( G_FUNC ) G_FUNC.get()

HRWatcher::HRWatcher()
	: _enabled( false )
	, _isConnected( false )
	, _device( nullptr )
	, _service( nullptr )
	, _characteristic( nullptr )
	, _descriptor( nullptr )
{
	_watcher.ScanningMode( BluetoothLEScanningMode::Active );

	{
		winrt::array_view<const uint8_t> ENABLE_NOTIFICATION_VALUE( { 0x01, 0x00 } );

		winrt::Windows::Storage::Streams::DataWriter wBuf;
		wBuf.WriteBytes( ENABLE_NOTIFICATION_VALUE );

		_enableNotification = wBuf.DetachBuffer();
	}
	{
		winrt::array_view<const uint8_t> DISABLE_NOTIFICATION_VALUE( { 0x00, 0x00 } );

		winrt::Windows::Storage::Streams::DataWriter wBuf;
		wBuf.WriteBytes( DISABLE_NOTIFICATION_VALUE );

		_disableNotification = wBuf.DetachBuffer();;
	}
}

HRWatcher::~HRWatcher()
{
}

void HRWatcher::Initialize()
{
	init_apartment( winrt::apartment_type::single_threaded );

	_watcher.Received( { this, &HRWatcher::Recieved } );
	_watcher.Stopped( { this, &HRWatcher::Stopped } );

	_enabled = true;
	Start();
}

void HRWatcher::Start()
{
	Stop();

	_watcher.Start();
}

void HRWatcher::Stop()
{
	if( _enabled )
	{
		if( _device )
			_device.ConnectionStatusChanged( nullptr );

		if( _characteristic )
			_characteristic.ValueChanged( nullptr );

		if( _descriptor )
			GATT_AWAIT( _descriptor.WriteValueAsync( _disableNotification ) );
	}

	_device = nullptr;
	_service = nullptr;
	_characteristic = nullptr;
	_descriptor = nullptr;

	_isConnected = false;
	_watcher.Stop();
}

__declspec(noinline) void HRWatcher::Stopped( BluetoothLEAdvertisementWatcher sender, BluetoothLEAdvertisementWatcherStoppedEventArgs args )
{
	if( _enabled && !_isConnected )
	{
		Start();
	}
}

__declspec(noinline) void HRWatcher::Recieved( BluetoothLEAdvertisementWatcher sender, BluetoothLEAdvertisementReceivedEventArgs args )
{
	try
	{
		const winrt::param::hstring localName = args.Advertisement().LocalName();
		if( !HeartRateMonitor::Instance().IsCorrectDevice( localName ) )
			return;

		_watcher.Stop();

		_device = GATT_AWAIT( BluetoothLEDevice::FromBluetoothAddressAsync( args.BluetoothAddress() ) );

		_device.ConnectionStatusChanged( { this, &HRWatcher::ConnectionStatusChanged } );

		GattDeviceServicesResult serviceResult{ GATT_AWAIT( _device.GetGattServicesAsync() ) };

		auto result = GATT_AWAIT( _device.GetGattServicesForUuidAsync( HeartRateMonitor::ServiceGUID ) );
		if( result.Status() != GattCommunicationStatus::Success )
		{
			throw std::exception();
		}


		auto services = result.Services();
		const uint32_t numServices = services.Size();
		for( uint32_t iService = 0; iService < numServices; ++iService )
		{
			_service = services.GetAt( iService );
			auto characteristicsResult = GATT_AWAIT( _service.GetCharacteristicsForUuidAsync( HeartRateMonitor::MeasurementGUID ) );
			if( characteristicsResult.Status() != GattCommunicationStatus::Success )
			{
				throw std::exception();
			}

			auto characteristics = characteristicsResult.Characteristics();

			const uint32_t numChar = characteristics.Size();
			for( uint32_t iChar = 0; iChar < numChar; ++iChar )
			{
				_characteristic = characteristics.GetAt( iChar );

				auto descriptorsResult = GATT_AWAIT( _characteristic.GetDescriptorsForUuidAsync( HeartRateMonitor::CharacteristicGUID ) );
				if( descriptorsResult.Status() != GattCommunicationStatus::Success )
				{
					throw std::exception();
				}

				auto descriptors = descriptorsResult.Descriptors();
				const uint32_t numDesc = descriptors.Size();
				for( uint32_t iDesc = 0; iDesc < numDesc; ++iDesc )
				{
					_descriptor = descriptors.GetAt( iDesc );

					GATT_AWAIT( _descriptor.WriteValueAsync( _enableNotification ) );

					_characteristic.ValueChanged( { this, &HRWatcher::ValueChanged } );
				}
			}
		}
	}
	catch( ... )
	{
		Start();
	}
}

__declspec(noinline) void HRWatcher::ConnectionStatusChanged( BluetoothLEDevice sender, IInspectable )
{
	if( sender.ConnectionStatus() == BluetoothConnectionStatus::Disconnected )
	{
		_isConnected = false;

		if( _enabled )
		{
			HeartRateMonitor::Instance().ProcessData( 0 );

			Start();
		}
	}
}

__declspec(noinline) void HRWatcher::ValueChanged( GattCharacteristic sender, GattValueChangedEventArgs args )
{
	const uint8_t * ptr = args.CharacteristicValue().data();
	if( ptr )
	{
		HeartRateMonitor::Instance().ProcessData( ptr[1] );
		_isConnected = true;
	}
}

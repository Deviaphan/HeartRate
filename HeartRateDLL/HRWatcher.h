#pragma once

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Devices::Bluetooth;
using namespace winrt::Windows::Devices::Bluetooth::Advertisement;
using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;

class HRWatcher
{
public:
	HRWatcher();
	~HRWatcher();

private:
	HRWatcher( const HRWatcher & ) = delete;
	HRWatcher & operator = ( const HRWatcher & ) = delete;

public:
	void Initialize();
	void Start();
	void Stop();

	void Recieved( BluetoothLEAdvertisementWatcher sender, BluetoothLEAdvertisementReceivedEventArgs args );
	void Stopped( BluetoothLEAdvertisementWatcher sender, BluetoothLEAdvertisementWatcherStoppedEventArgs args );
	
	void ConnectionStatusChanged( BluetoothLEDevice sender, IInspectable );
	void ValueChanged( GattCharacteristic sender, GattValueChangedEventArgs args );

	bool _enabled;

protected:
	BluetoothLEAdvertisementWatcher _watcher;

	BluetoothLEDevice _device;
	GattDeviceService _service;
	GattCharacteristic _characteristic;
	GattDescriptor _descriptor;

	bool _isConnected;

	winrt::Windows::Storage::Streams::IBuffer _enableNotification;
	winrt::Windows::Storage::Streams::IBuffer _disableNotification;

};

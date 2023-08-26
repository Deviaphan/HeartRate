#include "pch.h"
#include "HeartRateMonitor.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Devices::Bluetooth;
using namespace winrt::Windows::Devices::Bluetooth::Advertisement;
using namespace winrt::Windows::Devices::Bluetooth::GenericAttributeProfile;


HeartRateMonitor & HeartRateMonitor::Instance()
{
	static HeartRateMonitor hrm;

	return hrm;
}

constexpr uint32_t to_uint( const char value ) noexcept
{
	if( value >= '0' && value <= '9' )
	{
		return value - '0';
	}

	if( value >= 'A' && value <= 'F' )
	{
		return 10 + value - 'A';
	}

	if( value >= 'a' && value <= 'f' )
	{
		return 10 + value - 'a';
	}

	std::terminate();
}

constexpr guid make_guid( std::string_view const & value ) noexcept
{
	if( value.size() != 36 || value[8] != '-' || value[13] != '-' || value[18] != '-' || value[23] != '-' )
	{
		std::terminate();
	}

	return
	{
		((to_uint( value[0] ) * 16 + to_uint( value[1] )) << 24) +
		((to_uint( value[2] ) * 16 + to_uint( value[3] )) << 16) +
		((to_uint( value[4] ) * 16 + to_uint( value[5] )) << 8) +
		(to_uint( value[6] ) * 16 + to_uint( value[7] )),

		static_cast<uint16_t>(((to_uint( value[9] ) * 16 + to_uint( value[10] )) << 8) +
			(to_uint( value[11] ) * 16 + to_uint( value[12] ))),

		static_cast<uint16_t>(((to_uint( value[14] ) * 16 + to_uint( value[15] )) << 8) +
			(to_uint( value[16] ) * 16 + to_uint( value[17] ))),

		{
			static_cast<uint8_t>(to_uint( value[19] ) * 16 + to_uint( value[20] )),
			static_cast<uint8_t>(to_uint( value[21] ) * 16 + to_uint( value[22] )),

			static_cast<uint8_t>(to_uint( value[24] ) * 16 + to_uint( value[25] )),
			static_cast<uint8_t>(to_uint( value[26] ) * 16 + to_uint( value[27] )),
			static_cast<uint8_t>(to_uint( value[28] ) * 16 + to_uint( value[29] )),
			static_cast<uint8_t>(to_uint( value[30] ) * 16 + to_uint( value[31] )),
			static_cast<uint8_t>(to_uint( value[32] ) * 16 + to_uint( value[33] )),
			static_cast<uint8_t>(to_uint( value[34] ) * 16 + to_uint( value[35] )),
		}
	};
}

winrt::guid HeartRateMonitor::ServiceGUID;
winrt::guid HeartRateMonitor::MeasurementGUID;
winrt::guid HeartRateMonitor::CharacteristicGUID;

HeartRateMonitor::HeartRateMonitor()
	: _callback(nullptr)
	, _userPtr(nullptr)
{
	ServiceGUID = make_guid( "0000180d-0000-1000-8000-00805f9b34fb" );
	MeasurementGUID = make_guid( "00002a37-0000-1000-8000-00805f9b34fb" );
	CharacteristicGUID = make_guid( "00002902-0000-1000-8000-00805f9b34fb" );
}

HeartRateMonitor::~HeartRateMonitor()
{
}

void HeartRateMonitor::Initialize( const wchar_t * deviceName, HeartRate_CallbackFunction func, void * userPtr )
{
	_deviceName = deviceName;
	_callback = func;
	_userPtr = userPtr;
}

bool HeartRateMonitor::IsCorrectDevice( const winrt::hstring & deviceName ) const
{
	return deviceName == _deviceName;
}

void HeartRateMonitor::ProcessData( int hrData )
{
	_callback( hrData, _userPtr );
}

void HeartRateMonitor::StartMonitoring()
{
	_hrWatcher.Initialize();
}

void HeartRateMonitor::StopMonitoring()
{
	_hrWatcher._enabled = false;
	_hrWatcher.Stop();
}

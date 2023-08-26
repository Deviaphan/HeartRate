#pragma once

#include "HeartRate.h"
#include "HRWatcher.h"
#include <vector>

class HeartRateMonitor
{
public:
	static HeartRateMonitor & Instance();
	static winrt::guid ServiceGUID;
	static winrt::guid MeasurementGUID;
	static winrt::guid CharacteristicGUID;

private:
	HeartRateMonitor();

	HeartRateMonitor( const HeartRateMonitor  & ) = delete;
	HeartRateMonitor & operator =( const HeartRateMonitor & ) = delete;

public:
	~HeartRateMonitor();

public:
	void Initialize( const wchar_t * deviceName, HeartRate_CallbackFunction func, void * userPtr );
	bool IsCorrectDevice( const winrt::hstring & deviceName ) const;

	void ProcessData( int hrData );

	void StartMonitoring();
	void StopMonitoring();

private:

	HRWatcher _hrWatcher;

	winrt::hstring _deviceName;
	HeartRate_CallbackFunction _callback;
	void * _userPtr;
};

#include "pch.h"
#include "HeartRate.h"
#include "HeartRateMonitor.h"

extern "C"
{
	void __stdcall HeartRate_Initialize( const wchar_t * deviceName, HeartRate_CallbackFunction func, void * userPtr )
	{
		HeartRateMonitor::Instance().Initialize( deviceName, func, userPtr );
	}

	void __stdcall HeartRate_StartMonitoring()
	{
		HeartRateMonitor::Instance().StartMonitoring();
	}

	void __stdcall HeartRate_StopMonitoring()
	{
		HeartRateMonitor::Instance().StopMonitoring();
	}
}

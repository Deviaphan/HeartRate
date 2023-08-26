#pragma once

typedef void(*HeartRate_CallbackFunction)( int heartRate, void * userPtr);

extern "C"
{
	__declspec(dllexport) void __stdcall HeartRate_Initialize( const wchar_t * deviceName, HeartRate_CallbackFunction func, void * userPtr );

	__declspec(dllexport) void __stdcall HeartRate_StartMonitoring();
	__declspec(dllexport) void __stdcall HeartRate_StopMonitoring();
}

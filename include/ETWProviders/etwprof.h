//============ Copyright (c) Cygnus Software, All rights reserved. ============
//
// ETW (Event Tracing for Windows) profiling helpers.
// This allows easy insertion of Generic Event markers into ETW/xperf tracing
// which then aids in analyzing the traces and finding performance problems.
// The usage patterns are to use ETWBegin and ETWEnd (typically through the
// convenience class CETWScope) to bracket time-consuming operations. In addition
// ETWFrameMark marks the beginning of each frame, and ETWMark can be used to
// mark other notable events. More event types and providers can be added as needed.
//
//===============================================================================

#ifndef ETWPROF_H
#define ETWPROF_H
#if defined( _MSC_VER )
#pragma once
#endif

typedef long long int64;

#ifdef	_WIN32
// ETW support should be compiled in for all Windows PC platforms. It isn't
// supported on Windows XP but that is determined at run-time. This #define
// is used to let the code compile (but do nothing) on other operating systems.
#define	ETW_MARKS_ENABLED
#endif

// Flag to indicate that a mouse-down actually corresponds to a double-click.
// Add this to the button number.
const int kFlagDoubleClick = 100;

#ifdef	ETW_MARKS_ENABLED

#ifdef ETWPROVIDERSDLL
	#define PLATFORM_INTERFACE __declspec(dllexport)
#else
	#define PLATFORM_INTERFACE __declspec(dllimport)
#endif
#include <sal.h> // For _Printf_format_string_

// Insert a single event to mark a point in an ETW trace.
PLATFORM_INTERFACE void ETWMark( _In_z_ PCSTR pMessage );
// ETWWorkerMark is identical to ETWMark but goes through a different provider,
// for different grouping.
PLATFORM_INTERFACE void ETWWorkerMark(_In_z_ PCSTR pMessage);

// Insert events with one or more generic int or float data fields
PLATFORM_INTERFACE void ETWMark1I(_In_z_ PCSTR pMessage, int data1);
PLATFORM_INTERFACE void ETWMark2I(_In_z_ PCSTR pMessage, int data1, int data2);
PLATFORM_INTERFACE void ETWMark1F(_In_z_ PCSTR pMessage, float data1);
PLATFORM_INTERFACE void ETWMark2F(_In_z_ PCSTR pMessage, float data1, float data2);

// _Printf_format_string_ is used by /analyze
PLATFORM_INTERFACE void ETWMarkPrintf( _Printf_format_string_ _In_z_ PCSTR pMessage, ... );
PLATFORM_INTERFACE void ETWWorkerMarkPrintf( _Printf_format_string_ _In_z_ PCSTR pMessage, ... );

// Private Working Set, Proportional Set Size (shared memory charged proportionally, and total Working Set
// counter is just a number that allows grouping together all the working set samples collected at the same time.
PLATFORM_INTERFACE void ETWMarkWorkingSet(_In_z_ PCWSTR pProcessName, _In_z_ PCWSTR pProcess, unsigned counter, unsigned privateWS, unsigned PSS, unsigned workingSet);

// Record powerState (charging/discharging/AC), batteryPercentage (of total capacity) and
// discharge rate from struct BATTERY_STATUS.
PLATFORM_INTERFACE void ETWMarkBatteryStatus(_In_z_ PCSTR powerState, float batteryPercentage, _In_z_ PCSTR rate);

// Record CPU/package frequency, power usage, and temperature. Currently Intel only.
PLATFORM_INTERFACE void ETWMarkCPUFrequency(_In_z_ PCWSTR MSRName, double frequencyMHz);
PLATFORM_INTERFACE void ETWMarkCPUPower(_In_z_ PCWSTR MSRName, double powerW, double energymWh);
PLATFORM_INTERFACE void ETWMarkCPUTemp(_In_z_ PCWSTR MSRName, double tempC, double maxTempC);

// Insert a begin event to mark the start of some work. The return value is a 64-bit
// time stamp which should be passed to the corresponding ETWEnd function.
PLATFORM_INTERFACE int64 ETWBegin( _In_z_ PCSTR pMessage );
PLATFORM_INTERFACE int64 ETWWorkerBegin( _In_z_ PCSTR pMessage );

// Insert a paired end event to mark the end of some work.
PLATFORM_INTERFACE int64 ETWEnd( _In_z_ PCSTR pMessage, int64 nStartTime );
PLATFORM_INTERFACE int64 ETWWorkerEnd( _In_z_ PCSTR pMessage, int64 nStartTime );

// Mark the start of the next render frame.
PLATFORM_INTERFACE void ETWRenderFrameMark();
// Return the frame number recorded in the ETW trace -- useful for synchronizing
// other profile information to the ETW trace.
PLATFORM_INTERFACE int ETWGetRenderFrameNumber();

// Button numbers are 0, 1, 2 for left, middle, right, with kFlagDoubleClick added
// in for double clicks.
PLATFORM_INTERFACE void ETWMouseDown( int nWhichButton, unsigned flags, int nX, int nY );
PLATFORM_INTERFACE void ETWMouseUp( int nWhichButton, unsigned flags, int nX, int nY );
PLATFORM_INTERFACE void ETWMouseMove( unsigned flags, int nX, int nY );
PLATFORM_INTERFACE void ETWMouseWheel( unsigned flags, int zDelta, int nX, int nY );
PLATFORM_INTERFACE void ETWKeyDown( unsigned nChar, _In_opt_z_ const char* keyName, unsigned nRepCnt, unsigned flags );

// This class calls the ETW Begin and End functions in order to insert a
// pair of events to bracket some work.
class CETWScope
{
public:
	CETWScope( _In_z_ PCSTR pMessage )
		: m_pMessage( pMessage )
	{
		m_nStartTime = ETWBegin( pMessage );
	}
	~CETWScope()
	{
		ETWEnd( m_pMessage, m_nStartTime );
	}
private:
	// disable copying.
	CETWScope( const CETWScope& rhs ) = delete;
	CETWScope& operator=( const CETWScope& rhs ) = delete;

	_Field_z_ PCSTR m_pMessage;
	int64 m_nStartTime;
};

#else

// Portability macros to allow compiling on non-Windows platforms

inline void ETWMark( const char* ) {}
inline void ETWWorkerMark( const char *pMessage ) {}
inline void ETWMark1I(const char* pMessage, int data1) {}
inline void ETWMark2I(const char* pMessage, int data1, int data2) {}
inline void ETWMark1F(const char* pMessage, float data1) {}
inline void ETWMark2F(const char* pMessage, float data1, float data2) {}
inline void ETWMarkPrintf( const char *pMessage, ... ) {}
inline void ETWWorkerMarkPrintf( const char *pMessage, ... ) {}
inline void ETWMarkWorkingSet(const wchar_t* pProcessName, const wchar_t* pProcess, unsigned counter, unsigned privateWS, unsigned PSS, unsigned workingSet) {}
inline void ETWMarkBatteryStatus(_In_z_ PCSTR powerState, float batteryPercentage, _In_z_ PCSTR rate) {}
inline void ETWMarkCPUFrequency(_In_z_ PCSTR MSRName, double frequencyMHz) {}
inline void ETWMarkCPUPower(_In_z_ PCSTR MSRName, double powerW, double energymWh) {}
inline void ETWMarkCPUTemp(_In_z_ PCSTR MSRName, double tempC, double maxTempC) {}
inline int64 ETWBegin( const char* ) { return 0; }
inline int64 ETWWorkerBegin( const char* ) { return 0; }
inline int64 ETWEnd( const char*, int64 ) { return 0; }
inline int64 ETWWorkerEnd( const char*, int64 ) { return 0; }
inline void ETWRenderFrameMark() {}
inline int ETWGetRenderFrameNumber() { return 0; }

inline void ETWMouseDown( int nWhichButton, unsigned int flags, int nX, int nY ) {}
inline void ETWMouseUp( int nWhichButton, unsigned int flags, int nX, int nY ) {}
inline void ETWMouseMove( unsigned int flags, int nX, int nY ) {}
inline void ETWMouseWheel( unsigned int flags, int zDelta, int nX, int nY ) {}
inline void ETWKeyDown( unsigned nChar, const char* keyName, unsigned nRepCnt, unsigned flags ) {}

// This class calls the ETW Begin and End functions in order to insert a
// pair of events to bracket some work.
class CETWScope
{
public:
	CETWScope( const char* )
	{
	}
private:
	// disable copying.
	CETWScope( const CETWScope& rhs ) = delete;
	CETWScope& operator=( const CETWScope& rhs ) = delete;
};

#endif

#endif // ETWPROF_H

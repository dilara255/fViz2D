#pragma once

//Definições para controle dos projetos, sistemas, compilação, linkagem, etc
//TODO: A lot of this could all be on the build system. Is that preferable? 
//Either way, go all the way EITHER there OR here

#ifndef F_AUX
	#include "logAPI.hpp" 
#endif

#define BIT(x) (1 << x)
namespace AZ{ long long getExpectedWakeUpDelay(long long microsToSleep); }

#ifdef AS_PLATFORM_WINDOWS

	#ifdef F_VIZ2D
		#define	F_V2_API __declspec(dllexport)
	#else
		#define	F_V2_API __declspec(dllimport)
	#endif
	
	#define AZ_LOG_TRACE_COLOR (FOREGROUND_BLUE | FOREGROUND_GREEN)
	#define AZ_LOG_DEBUG_COLOR (FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY)
	#define AZ_LOG_INFO_COLOR (FOREGROUND_GREEN | FOREGROUND_INTENSITY)
	
	//Based on tests on a single machine, tuned for release mode with low concurrency
	inline long long AZ::getExpectedWakeUpDelay(long long microsToSleep) {
		//return (long long)round(478 - 0.00373*microsToSleep);
		return 0;
	}
#else
	#error no support for other platforms as of yet : /
#endif

/* These have all been moved to the build system

#ifdef AS_PLATFORM_WINDOWS
	#define SYSTEM_NAME "Windows" //Moved to the build system
#endif

#ifdef AS_DEBUG
	#define CONFIG_NAME "Debug"
#elif RELEASE 
	#define CONFIG_NAME "Release"
#endif

#ifdef X64
	#define PLATFORM_NAME "64 bits"
#elif X86
	#define PLATFORM_NAME "32 bits"
#endif

*/





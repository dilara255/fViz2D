#pragma once

//Defini��es para controle dos projetos, sistemas, compila��o, linkagem, etc
//TODO: A lot of this could all be on the build system. Is that preferable? 
//Either way, go all the way EITHER there OR here

//TODO: F_V2_API definition makes no sense here. Either use a single definition for all projects, or use make system for these definitions?

#ifndef F_AUX
	#include "logAPI.hpp" 
#endif

#define BIT(x) (1 << x)

namespace AZ{
	static long long getExpectedWakeUpDelay(long long microsToSleep = 0){
		//return (long long)round(478 - 0.00373*microsToSleep);
		return 0;
	}
}

///MOVE TO PLATFORM-LAYER:

#ifdef AS_PLATFORM_WINDOWS

	#ifdef F_VIZ2D
		#define	F_V2_API __declspec(dllexport)
	#else
		#define	F_V2_API __declspec(dllimport)
	#endif

	#define AZ_LOG_TRACE_COLOR (FOREGROUND_BLUE | FOREGROUND_GREEN)
	#define AZ_LOG_DEBUG_COLOR (FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY)
	#define AZ_LOG_INFO_COLOR (FOREGROUND_GREEN | FOREGROUND_INTENSITY)

#elif defined(F_OS_LINUX)

	//Try
	// #define	F_V2_API __attribute__ ((visibility ("default"))) ?
	#ifdef F_VIZ2D
		#define	F_V2_API
	#else
		#define	F_V2_API
	#endif

	const char azLogTraceColor[] = "\033[36m";
	const char azLogDebugColor[] = "\033[35m\033[1m";
	const char azLogInfoColor[] = "\033[32m\033[1m";

	#define AZ_LOG_TRACE_COLOR (azLogTraceColor)
	#define AZ_LOG_DEBUG_COLOR (azLogDebugColor)
	#define AZ_LOG_INFO_COLOR (azLogInfoColor)

#else
	#error "OS not recognized"
#endif

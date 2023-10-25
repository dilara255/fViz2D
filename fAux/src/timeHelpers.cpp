//TODO: this file had a #pragma once. THat was a mistake, right?

#include "timeHelpers.hpp"

#include "miscStdHeaders.h"

#define AZ_MICROS_TO_BUSY_WAIT 25
#define MICROS_IN_A_SECOND 1000000
#define MILLIS_IN_A_SECOND 1000

namespace AZ{

	//TODO: this is using dummy++ to make sure the compiler doesn't take the loop out'
	//Clang notices dummy is useless. Is this the best way to go about it?
	void hybridBusySleep(std::chrono::steady_clock::time_point targetWakeTime,
		                                  std::chrono::microseconds threshold) {

		int dummy = 0;
		static std::chrono::microseconds zeroMicro(0);

		while ((targetWakeTime - std::chrono::steady_clock::now()) > threshold) {
			std::this_thread::sleep_for(zeroMicro);
		}
		while (std::chrono::steady_clock::now() < targetWakeTime){dummy++;}
	}

	void hybridBusySleepForMicros(std::chrono::microseconds microsToSleep,
		                              std::chrono::microseconds threshold){

		auto targetWakeTime = std::chrono::steady_clock::now() + microsToSleep;

		hybridBusySleep(targetWakeTime, threshold);
	}
}

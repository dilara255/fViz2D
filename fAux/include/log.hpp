#pragma once

//Declaração funções log. Usa Spdlog. Exposto via macros em logAPI.hpp

#include "miscStdHeaders.h"

#include "core.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace az {

	class Log
	{
	public:
		static void init();
		inline static std::shared_ptr<spdlog::logger>& GetVZLogger() {
			if (!initialized) init();  return s_fViz2DLogger;
		}
		inline static std::shared_ptr<spdlog::logger>& GetTALogger() {
			if (!initialized) init(); return s_TestAppLogger;
		}
		inline static std::shared_ptr<spdlog::logger>& GetDALogger() {
			if (!initialized) init(); return s_DebugAuxLogger;
		}
		inline static void log(std::shared_ptr<spdlog::logger> logger, const int degree,
			                   const char* file, const int line, 
							   const char* message, uint32_t trailingNewlines = 0);

	private:
		static std::shared_ptr<spdlog::logger> s_fViz2DLogger;
		static std::shared_ptr<spdlog::logger> s_TestAppLogger;
		static std::shared_ptr<spdlog::logger> s_DebugAuxLogger;
		static int initialized;
	};

}


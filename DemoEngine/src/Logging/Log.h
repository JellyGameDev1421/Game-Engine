#pragma once

#include "Core/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace DemoEngine 
{
	class Log 
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}

#define LOG_TRACE(...) :: DemoEngine::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)  :: DemoEngine::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)  :: DemoEngine::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) :: DemoEngine::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...) :: DemoEngine::Log::GetLogger()->fatal(__VA_ARGS__)


//Core log macros thers act like shortcuts saving us from typing like logging to the console.
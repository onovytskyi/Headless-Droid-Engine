#pragma once

#include "Foundation/Memory/FixedLinearAllocator.h"

namespace hd
{
	enum class LogSeverity : uint8_t
	{
		Info,
		Warning,
		Error
	};

	extern FixedLinearAllocator g_LogAllocator;

	template<typename... Args>
	void Log(LogSeverity severity, char8_t const* format, Args&&... args);
	void Log(LogSeverity severity, char8_t const* message);

	void LogInternal(LogSeverity severity, char8_t const* message);
	void LogConsole(LogSeverity severity, wchar_t const* message, size_t messageLength);
	void LogFile(LogSeverity severity, wchar_t const* message);
	void LogDevelopmentEnvironment(LogSeverity severity, wchar_t const* message);
}

#include "Debug/Log.hpp"

#if defined(HD_ENABLE_LOG) && HD_LOG_LEVEL <= HD_LOG_LEVEL_INFO
#	define hdLogInfo(text, ...) hd::Log(hd::LogSeverity::Info, text, __VA_ARGS__)
#else
#	define hdLogInfo(text, ...) ((void)0)
#endif

#if defined(HD_ENABLE_LOG) && HD_LOG_LEVEL <= HD_LOG_LEVEL_WARNING
#	define hdLogWarning(text, ...) hd::Log(hd::LogSeverity::Warning, text, __VA_ARGS__)
#else
#	define hdLogWarning(text, ...) ((void)0)
#endif

#if defined(HD_ENABLE_LOG) && HD_LOG_LEVEL <= HD_LOG_LEVEL_ERROR
#	define hdLogError(text, ...) hd::Log(hd::LogSeverity::Error, text, __VA_ARGS__)
#else
#	define hdLogError(text, ...) ((void)0)
#endif

#include "Engine/Config/Bootstrap.h"

#include "Engine/Debug/Log.h"

#include "Engine/Foundation/Memory/Utils.h"
#include "Engine/Foundation/String/StringConverter.h"
#include "Engine/Foundation/String/StringFormatter.h"

namespace hd
{
    namespace dbg
    {
        static const size_t g_LogAllocatorSize = mem::MB(2);
        std::byte g_LogContextAllocatorMemory[g_LogAllocatorSize];
        mem::FixedLinearAllocator g_LogAllocator{ g_LogContextAllocatorMemory, g_LogAllocatorSize };

        static size_t g_GameStartTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        const char8_t* ToString(LogSeverity severity)
        {
            switch (severity)
            {
            case LogSeverity::Info: return u8"INFO";
            case LogSeverity::Warning: return u8"WARNING";
            case LogSeverity::Error: return u8"ERROR";
            }

            return u8"-";
        }

        void Log(LogSeverity severity, char8_t const* message)
        {
            LogInternal(severity, message);
        }

        void LogInternal(LogSeverity severity, char8_t const* text)
        {
            size_t marker = g_LogAllocator.GetMarker();

            char8_t const* format = u8"[%:%:%:%][%] %\n";
            auto timeSinceStart = std::chrono::system_clock::now().time_since_epoch() - std::chrono::milliseconds(g_GameStartTimestamp);
            int64_t hours = std::chrono::duration_cast<std::chrono::hours>(timeSinceStart).count();
            int64_t minutes = std::chrono::duration_cast<std::chrono::minutes>(timeSinceStart).count() - hours * 60;
            int64_t seconds = std::chrono::duration_cast<std::chrono::seconds>(timeSinceStart).count() - hours * 60 - minutes * 60;
            int64_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceStart).count() - hours * 60 - minutes * 60 - seconds * 1000;

            size_t messageBufferSize = str::CalculateBufferSize(format, hours, minutes, seconds, milliseconds, ToString(severity), text);
            char8_t* message = reinterpret_cast<char8_t*>(g_LogAllocator.Allocate(messageBufferSize, 1));
            str::Format(message, format, hours, minutes, seconds, milliseconds, ToString(severity), text);

            size_t messagWideSize = str::SizeAsWide(message);
            wchar_t* messageWide = reinterpret_cast<wchar_t*>(g_LogAllocator.Allocate(messagWideSize, 1));
            str::ToWide(message, messageWide, messagWideSize);

#if defined(HD_ENABLE_CONSOLE_LOG)
            LogConsole(severity, messageWide, messagWideSize / sizeof(wchar_t) - 1);
#endif
#if defined(HD_ENABLE_FILE_LOG)
            LogFile(severity, messageWide);
#endif
#if defined(HD_ENABLE_IDE_LOG)
            LogDevelopmentEnvironment(severity, messageWide);
#endif

            g_LogAllocator.Reset(marker);
        }

        static std::wofstream g_fileStream;
        void LogFile(LogSeverity severity, wchar_t const* message)
        {
            if (!g_fileStream.is_open())
            {
                size_t marker = g_LogAllocator.GetMarker();

                size_t filepathWideSize = str::SizeAsWide(cfg::LogFilepath());
                wchar_t* filepathWide = reinterpret_cast<wchar_t*>(g_LogAllocator.Allocate(filepathWideSize, 1));

                str::ToWide(cfg::LogFilepath(), filepathWide, filepathWideSize);

                //#TODO std filesystem makes allocations internally
                //Replace it with platform solution?
                if (!std::filesystem::path(filepathWide).parent_path().empty())
                {
                    std::filesystem::create_directories(std::filesystem::path(filepathWide).parent_path());
                }
                g_fileStream.open(filepathWide, std::ios::trunc);

                g_LogAllocator.Reset(marker);
            }

            g_fileStream << message;
        }
    }
}
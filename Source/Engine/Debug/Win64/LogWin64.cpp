#include "Engine/Config/Bootstrap.h"

#if defined(HD_PLATFORM_WIN64)

#include "Engine/Debug/Log.h"

namespace hd
{
    namespace dbg
    {
        static HANDLE g_ConsoleHandle;
        static WORD g_LastConsoleAttribute;

        WORD ToConsoleAttribute(LogSeverity severity)
        {
            switch (severity)
            {
            case LogSeverity::Info: return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
            case LogSeverity::Warning: return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            case LogSeverity::Error: return FOREGROUND_RED | FOREGROUND_INTENSITY;
            }

            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }

        void LogConsole(LogSeverity severity, wchar_t const* message, size_t messageLength)
        {
            if (g_ConsoleHandle == 0)
            {
                ::AllocConsole();
                g_ConsoleHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
            }

            WORD consoleAttribute = ToConsoleAttribute(severity);
            if (consoleAttribute != g_LastConsoleAttribute)
            {
                ::SetConsoleTextAttribute(g_ConsoleHandle, consoleAttribute);
                g_LastConsoleAttribute = consoleAttribute;
            }

            ::WriteConsoleW(g_ConsoleHandle, message, DWORD(messageLength), NULL, NULL);
        }

        void LogDevelopmentEnvironment(LogSeverity severity, wchar_t const* message)
        {
            if (::IsDebuggerPresent())
            {
                ::OutputDebugStringW(message);
            }
        }
    }
}

#endif
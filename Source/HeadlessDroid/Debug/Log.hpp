#include "Foundation/String/StringFormatter.h"

namespace hd
{
    namespace dbg
    {
        template<typename... Args>
        void Log(LogSeverity severity, char8_t const* format, Args&&... args)
        {
            size_t marker = g_LogAllocator.GetMarker();

            size_t messageBufferSize = str::CalculateBufferSize(format, std::forward<Args>(args)...);
            char8_t* message = reinterpret_cast<char8_t*>(g_LogAllocator.Allocate(messageBufferSize, 1));
            str::Format(message, format, std::forward<Args>(args)...);

            LogInternal(severity, message);

            g_LogAllocator.Reset(marker);
        }
    }
}

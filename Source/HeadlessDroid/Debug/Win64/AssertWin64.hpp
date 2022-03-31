#include "Foundation/String/StringConverter.h"
#include "Foundation/String/StringFormatter.h"

namespace hd
{
    namespace dbg
    {
        void AssertHandlerFinal(char8_t const* assertion, char8_t const* file, uint32_t line, char8_t const* message);

        template<typename... Args>
        inline void AssertHandler(HRESULT errorCode, char8_t const* assertion, char8_t const* file, uint32_t line, Args... args)
        {
            if (FAILED(errorCode))
            {
                AssertHandlerInternal(errorCode, assertion, file, line, std::forward<Args>(args)...);
            }
        }

        template<typename... Args>
        inline void LastErrorHandler(bool condition, char8_t const* assertion, char8_t const* file, uint32_t line, Args... args)
        {
            if (!condition)
            {
                AssertHandler(HRESULT_FROM_WIN32(GetLastError()), assertion, file, line, std::forward<Args>(args)...);
            }
        }

        template<typename... Args>
        inline void AssertHandlerInternal(HRESULT errorCode, char8_t const* assertion, char8_t const* file, uint32_t line, char8_t const* format, Args... args)
        {
            size_t marker = g_AssertAllocator.GetMarker();

            _com_error error(errorCode);
            wchar_t const* winMessageWide = error.ErrorMessage();

            size_t winMessageBufferSize = str::SizeAsNarrow(winMessageWide);
            char8_t* winMessage = reinterpret_cast<char8_t*>(g_AssertAllocator.Allocate(winMessageBufferSize, 1));

            str::ToNarrow(winMessageWide, winMessage, winMessageBufferSize);

            //recreate format string and add windows message there
            size_t newFormatSize = str::CalculateBufferSize(u8"%\n%", format, winMessage);
            char8_t* newFormat = reinterpret_cast<char8_t*>(g_AssertAllocator.Allocate(newFormatSize, 1));
            str::Format(newFormat, u8"%\n%", format, winMessage);

            size_t bufferSize = str::CalculateBufferSize(newFormat, std::forward<Args>(args)...);

            char8_t* message = reinterpret_cast<char8_t*>(g_AssertAllocator.Allocate(bufferSize, 1));
            str::Format(message, newFormat, std::forward<Args>(args)...);

            AssertHandlerFinal(assertion, file, line, message);

            g_AssertAllocator.Reset(marker);
        }
    }
}
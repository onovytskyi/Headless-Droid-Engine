#include "Engine/Config/Bootstrap.h"

#if defined(HD_PLATFORM_WIN64)

#include "Engine/Debug/Assert.h"
#include "Engine/Foundation/String/StringConverter.h"

namespace hd
{
    namespace dbg
    {
        void AssertHandlerInternal(HRESULT errorCode, char8_t const* assertion, char8_t const* file, uint32_t line)
        {
            size_t marker = g_AssertAllocator.GetMarker();

            _com_error error(errorCode);
            wchar_t const* messageWide = error.ErrorMessage();

            size_t messageBufferSize = str::SizeAsNarrow(messageWide);
            char8_t* message = reinterpret_cast<char8_t*>(g_AssertAllocator.Allocate(messageBufferSize, 1));

            str::ToNarrow(messageWide, message, messageBufferSize);

            AssertHandlerFinal(assertion, file, line, message);

            g_AssertAllocator.Reset(marker);
        }

        void BreakIntoDebugger()
        {
            if (::IsDebuggerPresent())
            {
                ::DebugBreak();
            }
        }
    }
}

#endif
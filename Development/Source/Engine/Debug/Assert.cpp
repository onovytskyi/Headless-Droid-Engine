#include "Engine/Config/Bootstrap.h"

#include "Engine/Debug/Assert.h"

#include "Engine/Foundation/Memory/Utils.h"
#include "Engine/Foundation/String/StringConverter.h"
#include "Engine/Foundation/String/StringFormatter.h"
#include "Engine/Debug/Log.h"

static const size_t g_AssertAllocatorSize = hd::mem::MB(2);
std::byte g_AssertAllocatorArena[g_AssertAllocatorSize];
hd::mem::FixedLinearAllocator hd::dbg::g_AssertAllocator{ g_AssertAllocatorArena, g_AssertAllocatorSize };

namespace hd
{
    namespace dbg
    {
        void AssertHandlerInternal(char8_t const* assertion, char8_t const* file, uint32_t line)
        {
            AssertHandlerFinal(assertion, file, line, nullptr);
        }

        void AssertHandlerFinal(char8_t const* assertion, char8_t const* file, uint32_t line, char8_t const* message)
        {
            size_t marker = g_AssertAllocator.GetMarker();

            const char8_t* errorFormat = u8"[ASSERT FAILED] %\n\t\tCondition: %, File: %(%)";
            size_t errorBufferSize = str::CalculateBufferSize(
                errorFormat,
                message != nullptr ? message : u8"No message",
                assertion,
                file,
                line);

            char8_t* error = reinterpret_cast<char8_t*>(g_AssertAllocator.Allocate(errorBufferSize, 1));
            str::Format(
                error,
                errorFormat,
                message != nullptr ? message : u8"No message",
                assertion,
                file,
                line);

            hdLogError(error);

            g_AssertAllocator.Reset(marker);

            BreakIntoDebugger();

            exit(EXIT_FAILURE);
        }
    }
}

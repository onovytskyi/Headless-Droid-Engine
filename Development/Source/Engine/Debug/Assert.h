#pragma once

#include "Engine/Debug/Win64/AssertWin64.h"

#include "Engine/Foundation/Memory/FixedLinearAllocator.h"

namespace hd
{
    namespace dbg
    {
        extern mem::FixedLinearAllocator g_AssertAllocator;

        template<typename... Args>
        void AssertHandler(bool condition, char8_t const* assertion, char8_t const* file, uint32_t line, Args... args);

        template<typename... Args>
        void AssertHandlerInternal(char8_t const* assertion, char8_t const* file, uint32_t line, char8_t const* format, Args... args);
        void AssertHandlerInternal(char8_t const* assertion, char8_t const* file, uint32_t line);
        void AssertHandlerFinal(char8_t const* assertion, char8_t const* file, uint32_t line, char8_t const* message);
    }
}

#include "Engine/Debug/Assert.hpp"

#if defined(HD_ENABLE_ASSERTS)
#	define hdAssert( test, ... ) do{hd::dbg::AssertHandler(test, (char8_t const*)hdMakeString(test), (char8_t const*)__FILE__, __LINE__, __VA_ARGS__);}while(0)

#else
#	define hdAssert( test, ... ) ((void)0)
#endif

#define hdEnsure( test, ... ) do{hd::dbg:::AssertHandler(test, (char8_t const*)hdMakeString(test), (char8_t const*)__FILE__, __LINE__, __VA_ARGS__);}while(0)

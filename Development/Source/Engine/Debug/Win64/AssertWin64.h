#pragma once

#if defined(HD_PLATFORM_WIN64)

#include "Engine/Foundation/Memory/FixedLinearAllocator.h"

namespace hd
{
    namespace dbg
    {
        extern mem::FixedLinearAllocator g_AssertAllocator;

        template<typename... Args>
        void AssertHandler(HRESULT errorCode, char8_t const* assertion, char8_t const* file, uint32_t line, Args... args);
        template<typename... Args>
        void LastErrorHandler(bool condition, char8_t const* assertion, char8_t const* file, uint32_t line, Args... args);

        template<typename... Args>
        void AssertHandlerInternal(HRESULT errorCode, char8_t const* assertion, char8_t const* file, uint32_t line, char8_t const* format, Args... args);
        void AssertHandlerInternal(HRESULT errorCode, char8_t const* assertion, char8_t const* file, uint32_t line);

        void ShowErrorMessage(char8_t const* message);
        void BreakIntoDebugger();
    }
}

#include "Engine/Debug/Win64/AssertWin64.hpp"

#define hdCheckLastError( test, ... ) do{hd::dbg::LastErrorHandler(test, (char8_t const*)hdMakeString(test), (char8_t const*)__FILE__, __LINE__, __VA_ARGS__);}while(0)

#endif
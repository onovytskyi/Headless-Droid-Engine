#pragma once

#if defined(HD_PLATFORM_WIN64)

#include "Foundation/Memory/FixedLinearAllocator.h"

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

#if defined(HD_GRAPHICS_API_DX12) && defined(HD_ENABLE_GFX_DEBUG)
        void SetDebugDevice(ID3D12Device* device);
        HRESULT ConvertToGfxDeviceResult(HRESULT errorCode);
        void D3D12DebugMessageCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severiry, D3D12_MESSAGE_ID id, LPCSTR description, void* context);
#endif
    }
}

#include "Debug/Win64/AssertWin64.hpp"

#define hdCheckLastError( test, ... ) do{hd::dbg::LastErrorHandler(test, (char8_t const*)hdMakeString(test), (char8_t const*)__FILE__, __LINE__, __VA_ARGS__);}while(0)

#endif
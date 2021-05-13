#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace sys
    {
        class SystemWindow;
    }

    namespace gfx
    {
        class Backend;
        class Device;
        class Fence;
        class Queue;

        class SwapchainPlatform
        {
        public:
            SwapchainPlatform(Backend& backend, Device& device, Queue& queue, sys::SystemWindow& window, GraphicFormat format, mem::AllocationScope& allocationScope);
            ~SwapchainPlatform();

        protected:
            void UpdateGPUFrame();

            void CreateFramebufferTextures();
            void ReleaseFrameBufferTextures();

            ComPtr<IDXGISwapChain3> m_SwapChain;
            Device* m_OwnerDevice;
            Queue* m_FlipQueue;
            GraphicFormat m_Format;
            uint32_t m_FramebufferIndex;
            TextureHandle m_FramebufferTextures[cfg::MaxFrameLatency()];
            Fence* m_FrameFence;
            uint64_t m_CPUFrame;
            uint64_t m_GPUFrame;
        };
    }
}

#endif
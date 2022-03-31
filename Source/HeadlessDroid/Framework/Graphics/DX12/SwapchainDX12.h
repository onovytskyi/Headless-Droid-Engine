#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Graphics/GraphicsTypes.h"

namespace hd
{
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
            SwapchainPlatform(Allocator& persistentAllocator, Backend& backend, Device& device, Queue& queue, sys::SystemWindow& window, GraphicFormat format);
            ~SwapchainPlatform();

        protected:
            void UpdateGPUFrame();

            void CreateFramebufferTextures();
            void ReleaseFrameBufferTextures();

            Allocator& m_PersistentAllocator;
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
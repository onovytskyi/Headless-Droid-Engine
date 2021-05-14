#pragma once

#include "Engine/Framework/Graphics/DX12/SwapchainDX12.h"

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
        class Queue;

        class Swapchain : public SwapchainPlatform
        {
        public:
            Swapchain(Backend& backend, Device& device, Queue& queue, sys::SystemWindow& window, GraphicFormat format, mem::AllocationScope& allocationScope);
            ~Swapchain();

            hdNoncopyable(Swapchain)

            void Flip();
            void Resize(uint32_t width, uint32_t height);

            TextureHandle GetActiveFramebuffer() const;

            uint64_t GetCPUFrame() const;
            uint64_t GetGPUFrame() const;
        };
    }
}
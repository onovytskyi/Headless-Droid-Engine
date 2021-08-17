#pragma once

#include "Engine/Framework/Graphics/DX12/SwapchainDX12.h"

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    class Allocator;

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
            Swapchain(Allocator& persistentAllocator, Backend& backend, Device& device, Queue& queue, sys::SystemWindow& window, GraphicFormat format);
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
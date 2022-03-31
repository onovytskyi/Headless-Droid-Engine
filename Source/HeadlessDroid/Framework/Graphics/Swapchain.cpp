#include "Config/Bootstrap.h"

#include "Framework/Graphics/Swapchain.h"

namespace hd
{
    namespace gfx
    {
        Swapchain::Swapchain(Allocator& persistentAllocator, Backend& backend, Device& device, Queue& queue, sys::SystemWindow& window, GraphicFormat format)
            : SwapchainPlatform{ persistentAllocator, backend, device, queue, window, format }
        {

        }

        Swapchain::~Swapchain()
        {

        }

    }
}
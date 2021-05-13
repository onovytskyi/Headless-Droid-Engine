#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/Swapchain.h"

namespace hd
{
    namespace gfx
    {
        Swapchain::Swapchain(Backend& backend, Device& device, Queue& queue, sys::SystemWindow& window, GraphicFormat format, mem::AllocationScope& allocationScope)
            : SwapchainPlatform{ backend, device, queue, window, format, allocationScope }
        {

        }

        Swapchain::~Swapchain()
        {

        }

    }
}
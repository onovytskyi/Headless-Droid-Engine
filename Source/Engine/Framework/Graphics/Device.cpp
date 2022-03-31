#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/Device.h"

namespace hd
{
    namespace gfx
    {
        Device::Device(Allocator& persistentAllocator, Allocator& generalAllocator, Backend& backend)
            : DevicePlatform{ persistentAllocator, generalAllocator, backend }
        {

        }

        Device::~Device()
        {
            RecycleResources(std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max());
        }
    }
}
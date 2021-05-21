#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/Device.h"

namespace hd
{
    namespace gfx
    {
        Device::Device(Backend& backend, mem::AllocationScope& allocationScope)
            : DevicePlatform{ backend, allocationScope }
        {

        }

        Device::~Device()
        {
            RecycleResources(std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max());
        }
    }
}
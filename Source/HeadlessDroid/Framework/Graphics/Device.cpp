#include "Config/Bootstrap.h"

#include "Framework/Graphics/Device.h"

namespace hd
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

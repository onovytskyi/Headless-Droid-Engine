#include "Config/Bootstrap.h"

#include "Framework/Graphics/Fence.h"

namespace hd
{
	Fence::Fence(Device& device, uint64_t initialValue)
		: FencePlatform{ device, initialValue }
	{

	}

	Fence::~Fence()
	{

	}
}

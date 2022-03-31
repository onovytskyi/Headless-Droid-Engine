#include "Config/Bootstrap.h"

#include "Framework/Graphics/Backend.h"

namespace hd
{
	Backend::Backend(Allocator& persistentAllocator)
		: BackendPlatform{ persistentAllocator }
	{

	}

	Backend::~Backend()
	{

	}
}

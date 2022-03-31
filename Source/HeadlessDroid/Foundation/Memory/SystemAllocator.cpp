#include "Config/Bootstrap.h"

#include "Foundation/Memory/SystemAllocator.h"

namespace hd
{
	SystemAllocator::SystemAllocator()
	{

	}

	void* SystemAllocator::Allocate(size_t size, size_t align)
	{
	    return malloc(size);
	}

	void SystemAllocator::Deallocate(void* memory, size_t sizeInBytes, size_t alignInBytes)
	{
	    free(memory);
	}	
}

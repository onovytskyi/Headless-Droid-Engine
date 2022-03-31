#include "Config/Bootstrap.h"

#include "Foundation/Memory/SystemAllocator.h"

hd::SystemAllocator::SystemAllocator()
{

}

void* hd::SystemAllocator::Allocate(size_t size, size_t align)
{
    return malloc(size);
}

void hd::SystemAllocator::Deallocate(void* memory, size_t sizeInBytes, size_t alignInBytes)
{
    free(memory);
}

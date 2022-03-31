#pragma once

#include "Framework/Memory/ScopedScratchMemory.h"

namespace hd
{
    class Allocator;
    class AllocatorMemoryResource;

    Allocator& Scratch();
    size_t GetScratchMarker();
    void ResetScratchMarker(size_t marker);
}
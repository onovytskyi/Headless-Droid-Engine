#include "Game/Bootstrap.h"

#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Framework/Memory/LinearAllocatorConcurrentAdapter.h"
#include "Engine/Framework/Memory/VirtualLinearAllocator.h"
#include "Engine/Foundation/Memory/FixedLinearAllocator.h"
#include "Engine/Foundation/Memory/Utils.h"

hd::mem::LinearAllocator& hd::mem::GetScratchAllocator()
{
    static const size_t memoryArenaSize = MB(50);
    static thread_local std::byte memoryArena[memoryArenaSize];
    static thread_local FixedLinearAllocator memoryAllocator{ memoryArena, memoryArenaSize };

    return memoryAllocator;
}

hd::mem::LinearAllocator& hd::mem::GetPersistentAllocator()
{
    static VirtualLinearAllocator memoryAllocator{ GB(1) };
    static LinearAllocatorConcurrentAdaper concurrentAdapter{ memoryAllocator };

    return concurrentAdapter;
}

hd::mem::LinearAllocator& hd::mem::GetSceneAllocator()
{
    static VirtualLinearAllocator memoryAllocator{ GB(4) };
    static LinearAllocatorConcurrentAdaper concurrentAdapter{ memoryAllocator };

    return concurrentAdapter;
}

hd::mem::LinearAllocator& hd::mem::GetFrameAllocator()
{
    static VirtualLinearAllocator memoryAllocator{ GB(3), MB(500), false };
    static LinearAllocatorConcurrentAdaper concurrentAdapter{ memoryAllocator };

    return concurrentAdapter;
}
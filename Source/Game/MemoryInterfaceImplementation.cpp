#include "Game/Bootstrap.h"

#include "Engine/Memory/EngineMemoryInterface.h"
#include "Foundation/Memory/FixedLinearAllocator.h"
#include "Foundation/Memory/SystemAllocator.h"
#include "Foundation/Memory/Utils.h"
#include "Framework/Memory/AllocatorConcurrentAdapter.h"
#include "Framework/Memory/VirtualLinearAllocator.h"

static const size_t scratchArenaSize = hd::MB(50);
static thread_local std::byte scratchArena[scratchArenaSize];
static thread_local hd::FixedLinearAllocator scratchAllocator{ scratchArena, scratchArenaSize };

hd::Allocator& hd::Scratch()
{
    return scratchAllocator;
}

size_t hd::GetScratchMarker()
{
    return scratchAllocator.GetMarker();
}

void hd::ResetScratchMarker(size_t marker)
{
    scratchAllocator.Reset(marker);
}

static hd::VirtualLinearAllocator persistentAllocatorST{ hd::GB(1) };
static hd::AllocatorConcurrentAdaper persistentAllocatorMT{ persistentAllocatorST };

hd::Allocator& hd::Persistent()
{
    return persistentAllocatorMT;
}

static hd::SystemAllocator generalAllocatorMT{};

hd::Allocator& hd::General()
{
    return generalAllocatorMT;
}
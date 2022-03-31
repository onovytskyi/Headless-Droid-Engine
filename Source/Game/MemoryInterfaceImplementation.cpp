#include "Game/Bootstrap.h"

#include "Engine/Memory/EngineMemoryInterface.h"
#include "Foundation/Memory/FixedLinearAllocator.h"
#include "Foundation/Memory/SystemAllocator.h"
#include "Foundation/Memory/Utils.h"
#include "Framework/Memory/AllocatorConcurrentAdapter.h"
#include "Framework/Memory/VirtualLinearAllocator.h"

static const size_t scratchArenaSize = hd::mem::MB(50);
static thread_local std::byte scratchArena[scratchArenaSize];
static thread_local hd::mem::FixedLinearAllocator scratchAllocator{ scratchArena, scratchArenaSize };

hd::Allocator& hd::mem::Scratch()
{
    return scratchAllocator;
}

size_t hd::mem::GetScratchMarker()
{
    return scratchAllocator.GetMarker();
}

void hd::mem::ResetScratchMarker(size_t marker)
{
    scratchAllocator.Reset(marker);
}

static hd::mem::VirtualLinearAllocator persistentAllocatorST{ hd::mem::GB(1) };
static hd::mem::AllocatorConcurrentAdaper persistentAllocatorMT{ persistentAllocatorST };

hd::Allocator& hd::mem::Persistent()
{
    return persistentAllocatorMT;
}

static hd::SystemAllocator generalAllocatorMT{};

hd::Allocator& hd::mem::General()
{
    return generalAllocatorMT;
}
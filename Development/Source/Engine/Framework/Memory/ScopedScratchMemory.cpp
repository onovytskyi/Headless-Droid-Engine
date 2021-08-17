#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Memory/ScopedScratchMemory.h"
#include "Engine/Framework/Memory/FrameworkMemoryInterface.h"

namespace hd
{
    ScopedScratchMemory::ScopedScratchMemory()
        : m_Marker{ mem::GetScratchMarker() }
    {
    }

    ScopedScratchMemory::~ScopedScratchMemory()
    {
        mem::ResetScratchMarker(m_Marker);
    }
}
#include "Config/Bootstrap.h"

#include "Framework/Memory/ScopedScratchMemory.h"
#include "Framework/Memory/FrameworkMemoryInterface.h"

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
#include "Config/Bootstrap.h"

#include "Framework/Memory/ScopedScratchMemory.h"
#include "Framework/Memory/FrameworkMemoryInterface.h"

namespace hd
{
    ScopedScratchMemory::ScopedScratchMemory()
        : m_Marker{ GetScratchMarker() }
    {
    }

    ScopedScratchMemory::~ScopedScratchMemory()
    {
        ResetScratchMarker(m_Marker);
    }
}
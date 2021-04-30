#pragma once

#include "Engine/Framework/Memory/FrameworkMemoryInterface.h"

namespace hd
{
    namespace mem
    {
        class LinearAllocator;

        LinearAllocator& GetPersistentAllocator();
        LinearAllocator& GetSceneAllocator();
        LinearAllocator& GetFrameAllocator();
    }
}
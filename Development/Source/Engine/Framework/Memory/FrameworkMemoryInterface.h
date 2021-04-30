#pragma once

namespace hd
{
    namespace mem
    {
        class LinearAllocator;

        LinearAllocator& GetScratchAllocator();
    }
}
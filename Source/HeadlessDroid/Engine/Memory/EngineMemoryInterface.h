#pragma once

#include "Framework/Memory/FrameworkMemoryInterface.h"

namespace hd
{
    class Allocator;

    namespace mem
    {
        Allocator& Persistent();
        Allocator& General();
    }
}
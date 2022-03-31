#pragma once

#include "Framework/Graphics/DX12/BackendDX12.h"

namespace hd
{
    class Allocator;

    class Backend : public BackendPlatform
    {
    public:
        Backend(Allocator& persistentAllocator);
        ~Backend();

        hdNoncopyable(Backend)
    };
}
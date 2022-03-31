#pragma once

#include "Engine/Framework/Graphics/DX12/BackendDX12.h"

namespace hd
{
    class Allocator;

    namespace gfx
    {
        class Backend : public BackendPlatform
        {
        public:
            Backend(Allocator& persistentAllocator);
            ~Backend();

            hdNoncopyable(Backend)
        };
    }
}
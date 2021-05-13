#pragma once

#include "Engine/Framework/Graphics/DX12/BackendDX12.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace gfx
    {
        class Backend : public BackendPlatform
        {
        public:
            Backend();
            ~Backend();

            hdNoncopyable(Backend)
        };
    }
}
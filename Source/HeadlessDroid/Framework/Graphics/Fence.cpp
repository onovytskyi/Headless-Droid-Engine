#include "Config/Bootstrap.h"

#include "Framework/Graphics/Fence.h"

namespace hd
{
    namespace gfx
    {
        Fence::Fence(Device& device, uint64_t initialValue)
            : FencePlatform{ device, initialValue }
        {

        }

        Fence::~Fence()
        {

        }
    }
}
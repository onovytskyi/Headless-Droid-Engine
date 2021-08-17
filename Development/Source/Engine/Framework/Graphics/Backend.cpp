#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/Backend.h"

namespace hd
{
    namespace gfx
    {
        Backend::Backend(Allocator& persistentAllocator)
            : BackendPlatform{ persistentAllocator }
        {

        }

        Backend::~Backend()
        {

        }
    }
}
#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/Backend.h"

namespace hd
{
    namespace gfx
    {
        Backend::Backend(mem::AllocationScope& allocationScope)
            : BackendPlatform{ allocationScope }
        {

        }

        Backend::~Backend()
        {

        }
    }
}
#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/RenderState.h"

namespace hd
{
    namespace gfx
    {
        RenderState::RenderState(Backend& backend)
            : RenderStatePlatform{ backend }
        {

        }

        RenderState::~RenderState()
        {

        }
    }
}
#include "Config/Bootstrap.h"

#include "Framework/Graphics/RenderState.h"

namespace hd
{
	RenderState::RenderState(Backend& backend)
		: RenderStatePlatform{ backend }
	{

	}

	RenderState::~RenderState()
	{

	}
}

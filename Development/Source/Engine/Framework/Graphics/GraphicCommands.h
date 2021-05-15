#pragma once

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace util
    {
        class CommandBuffer;
        class CommandBufferReader;
    }

    namespace gfx
    {
        enum class GraphicCommandType : uint32_t
        {
            ClearRenderTarget
        };

        struct ClearRenderTargetCommandCommand
        {
            static ClearRenderTargetCommandCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static ClearRenderTargetCommandCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            TextureHandle Tagert;
            std::array<float, 4> Color;
        };
    }
}
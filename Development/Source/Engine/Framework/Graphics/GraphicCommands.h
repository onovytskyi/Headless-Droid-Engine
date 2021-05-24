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
            ClearRenderTarget,
            UpdateTexture
        };

        struct ClearRenderTargetCommand
        {
            static ClearRenderTargetCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static ClearRenderTargetCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            TextureHandle Target;
            std::array<float, 4> Color;
        };

        struct UpdateTextureCommand
        {
            static UpdateTextureCommand& WriteTo(util::CommandBuffer& commandBuffer, size_t dataSize);
            static UpdateTextureCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            TextureHandle Target;
            uint32_t FirstSubresource;
            uint32_t NumSubresources;
            std::byte* Data;
            size_t Size;
        };
    }
}
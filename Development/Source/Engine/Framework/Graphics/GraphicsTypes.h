#pragma once

#include "Engine/Framework/Utils/Handle.h"

namespace hd
{
    namespace gfx
    {
        struct BufferTag {};
        using BufferHandle = util::Handle<BufferTag, uint64_t, std::numeric_limits<uint64_t>::max()>;

        struct TextureTag {};
        using TextureHandle = util::Handle<TextureTag, uint64_t, std::numeric_limits<uint64_t>::max()>;

        enum class QueueType : uint32_t
        {
            Graphics,
            Compute,
            Copy
        };

        enum class GraphicFormat : uint32_t
        {
            RGBA8UNorm_Srgb
        };

        enum class TextureFlags : uint32_t
        {
            RenderTarget = 1 << 0,
            DepthStencil = 1 << 1,
            ShaderResource = 1 << 2,
            UnorderedAccess = 1 << 3
        };

        enum class TextureDimenstion : uint32_t
        {
            Texture1D,
            Texture2D,
            Texture3D,
            TextureCube
        };
    }
}
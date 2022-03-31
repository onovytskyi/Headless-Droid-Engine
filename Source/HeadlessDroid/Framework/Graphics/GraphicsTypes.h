#pragma once

#include "Framework/Utils/Flags.h"
#include "Framework/Utils/Handle.h"

namespace hd
{
    namespace gfx
    {
        struct BufferTag {};
        using BufferHandle = util::Handle<BufferTag, uint64_t, std::numeric_limits<uint64_t>::max()>;

        struct TextureTag {};
        using TextureHandle = util::Handle<TextureTag, uint64_t, std::numeric_limits<uint64_t>::max()>;

        extern BufferHandle INVALID_BUFFER_HANDLE;
        extern TextureHandle INVALID_TEXTURE_HANDLE;

        static const uint32_t ALL_SUBRESOURCES = std::numeric_limits<uint32_t>::max();

        enum class QueueType : uint32_t
        {
            Graphics,
            Compute,
            Copy
        };

        enum class GraphicFormat : uint32_t
        {
            Unknown,

            D24UNorm_S8UInt,
            RGBA8UNorm,
            RGBA8UNorm_Srgb,
            BGRA8Unorm,
            BGRA8Unorm_Srgb,
            BGRX8Unorm,
            BGRX8Unorm_Srgb,
            BC1Unorm,
            BC2Unorm,
            BC3Unorm,
            BC4Unorm,
            BC4Snorm,
            BC5Unorm,
            BC5Snorm,
            RGBA16Float,
            RGBA32Float
        };

        enum class PrimitiveType : uint32_t
        {
            Undefined,

            Point,
            Line,
            Triangle,
            Patch
        };

        enum class TopologyType : uint32_t
        {
            Undefined,

            List,
            Strip,
            ListAdjacent,
            StipAdjacent
        };

        enum class BlendType : uint32_t
        {
            None,

            Alpha,
            AlphaSource
        };

        enum class ComparisonFunc : uint32_t
        {
            Never,
            Less,
            Equal,
            LessEqual,
            Greater,
            NotEqual,
            GreaterEqual,
            Always
        };

        enum class BufferFlagsBits : uint32_t
        {
            ConstantBuffer,
            ShaderResource,
            UnorderedAccess,
            Transient
        };
        using BufferFlags = hd::util::Flags<BufferFlagsBits>;
        hdAllowFlagsForEnum(BufferFlagsBits);

        enum class TextureFlagsBits : uint32_t
        {
            RenderTarget,
            DepthStencil,
            ShaderResource,
            UnorderedAccess
        };
        using TextureFlags = hd::util::Flags<TextureFlagsBits>;
        hdAllowFlagsForEnum(TextureFlagsBits);

        enum class TextureDimenstion : uint32_t
        {
            Texture1D,
            Texture2D,
            Texture3D,
            TextureCube
        };

        struct Viewport
        {
            float MinX;
            float MaxX;
            float MinY;
            float MaxY;
            float MinZ;
            float MaxZ;
        };

        struct Rect
        {
            uint32_t MinX;
            uint32_t MaxX;
            uint32_t MinY;
            uint32_t MaxY;
        };
    }
}
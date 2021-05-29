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

            RGBA8UNorm,
            RGBA8UNorm_Srgb
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

        enum class BufferFlags : uint32_t
        {
            ConstantBuffer = 1 << 0,
            ShaderResource = 1 << 1,
            UnorderedAccess = 1 << 2,
            Transient = 1 << 3
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
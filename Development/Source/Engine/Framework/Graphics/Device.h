#pragma once

#include "Engine/Framework/Graphics/DX12/DeviceDX12.h"

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace gfx
    {
        class Backend;

        class Device : public DevicePlatform
        {
        public:
            Device(Backend& backend, mem::AllocationScope& allocationScope);
            ~Device();

            hdNoncopyable(Device)

            BufferHandle CreateBuffer(uint32_t numElements, uint32_t elementSize, BufferFlags flags);
            void DestroyBuffer(BufferHandle handle);
            void DestroyBufferImmediate(BufferHandle handle);

            uint32_t GetCBVShaderIndex(BufferHandle handle);
            uint32_t GetSRVShaderIndex(BufferHandle handle);
            uint32_t GetUAVShaderIndex(BufferHandle handle);

            TextureHandle CreateTexture(uint64_t width, uint32_t height, uint16_t depth, uint16_t mipLevels, GraphicFormat format, TextureFlags flags, TextureDimenstion dimension, 
                float clearValue[4]);
            void DestroyTexture(TextureHandle handle);
            void DestroyTextureImmediate(TextureHandle handle);

            uint32_t GetSRVShaderIndex(TextureHandle handle);
            uint32_t GetUAVShaderIndex(TextureHandle handle);

            void GetTextureDimensions(TextureHandle handle, uint64_t& outWidth, uint32_t& outHeight);

            void RecycleResources(uint64_t currentMarker, uint64_t completedMarker);

            void GetMemoryBudgets(size_t& outLocalBudget, size_t& outLocalUsage, size_t& outNonlocalBudget, size_t& outNonlocalUsage);
        };
    }
}
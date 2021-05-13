#pragma once

#include "Engine/Framework/Graphics/DX12/DeviceDX12.h"

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace gfx
    {
        class Backend;

        class Device : public DevicePlatform
        {
        public:
            Device(Backend& backend);
            ~Device();

            hdNoncopyable(Device)

            TextureHandle CreateTexture(uint64_t width, uint32_t height, uint16_t depth, uint16_t mipLevels, GraphicFormat format, uint32_t flags, TextureDimenstion dimension, float clearValue[4]);
            void DestroyTexture(TextureHandle handle);
            void DestroyTextureImmediate(TextureHandle handle);
        };
    }
}
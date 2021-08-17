#pragma once

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    class Allocator;

    namespace gfx
    {
        class Device;
    }

    namespace util
    {
        class CommandBuffer;
    }

    namespace render
    {
        class TextureCollection
        {
        public:
            TextureCollection(gfx::Device& device);
            ~TextureCollection();

            gfx::TextureHandle UploadTexture(char8_t const* textureFilePath, util::CommandBuffer& graphicsCommands);

        private:
            gfx::Device& m_Device;
            std::pmr::unordered_map<std::pmr::u8string, gfx::TextureHandle> m_Textures;
        };
    }
}
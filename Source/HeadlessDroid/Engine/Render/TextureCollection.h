#pragma once

#include "Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    class Allocator;
	class CommandBuffer;
	class Device;

    class TextureCollection
    {
    public:
        TextureCollection(Device& device);
        ~TextureCollection();

        TextureHandle UploadTexture(char8_t const* textureFilePath, CommandBuffer& graphicsCommands);

    private:
        Device& m_Device;
        std::pmr::unordered_map<std::pmr::u8string, TextureHandle> m_Textures;
    };
}
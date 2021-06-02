#pragma once

#include "Engine/Framework/Graphics/GraphicsTypes.h"
#include "Engine/Framework/String/String.h"

namespace hd
{
    namespace gfx
    {
        class Device;
    }

    namespace mem
    {
        class AllocationScope;
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
            TextureCollection(mem::AllocationScope& allocationScope, gfx::Device& device);
            ~TextureCollection();

            gfx::TextureHandle UploadTexture(char8_t const* textureFilePath, util::CommandBuffer& graphicsCommands);

        private:
            struct TextureHolder
            {
                TextureHolder(mem::AllocationScope& allocationScope);

                str::String TextureKey;
                gfx::TextureHandle Handle;

                TextureHolder* Next;
            };

            mem::AllocationScope& m_AllocationScope;
            gfx::Device& m_Device;
            TextureHolder* m_FirstTextureHolder;
        };
    }
}
#include "Engine/Config/Bootstrap.h"

#include "Engine/Engine/Render/TextureCollection.h"

#include "Engine/Debug/Log.h"
#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Engine/Utils/ResourceLoader.h"
#include "Engine/Framework/Graphics/Device.h"
#include "Engine/Framework/Graphics/GraphicCommands.h"
#include "Engine/Framework/Memory/PlainDataArray.h"
#include "Engine/Framework/Utils/CommandBuffer.h"

namespace hd
{
    namespace render
    {
        TextureCollection::TextureCollection(gfx::Device& device)
            : m_Device{ device }
            , m_Textures{ &mem::General() }
        {

        }

        TextureCollection::~TextureCollection()
        {
            for (auto& [textureName, textureHandle] : m_Textures)
            {
                m_Device.DestroyTexture(textureHandle);
            }
        }

        hd::gfx::TextureHandle TextureCollection::UploadTexture(char8_t const* textureFilePath, util::CommandBuffer& graphicsCommands)
        {
            ScopedScratchMemory scopedScratch{};

            Allocator& scratch = mem::Scratch();

            std::pmr::u8string textureFilePathStr(textureFilePath, &mem::Scratch());

            auto textureItem = m_Textures.find(textureFilePathStr);
            if (textureItem != m_Textures.end())
            {
                return textureItem->second;
            }

            PlainDataArray<std::byte> textureData{ &mem::Scratch() };
            util::ImageResource textureDesc{};
            util::LoadImage(textureFilePath, textureData, textureDesc);

            gfx::TextureHandle handle = m_Device.CreateTexture(textureDesc.Width, textureDesc.Height, 1, textureDesc.MipCount, textureDesc.Format, gfx::TextureFlagsBits::ShaderResource,
                textureDesc.IsCube ? gfx::TextureDimenstion::TextureCube : gfx::TextureDimenstion::Texture2D, nullptr);

            gfx::GraphicCommandsStream commandStream{ graphicsCommands };
            commandStream.UpdateTexture(handle, 0, gfx::ALL_SUBRESOURCES, textureData.Data(), textureData.Size());
            commandStream.UseAsReadableResource(handle);

            hdLogInfo(u8"Texture % loaded.", textureFilePathStr.c_str());

            m_Textures[textureFilePathStr] = handle;

            return handle;
        }
    }
}
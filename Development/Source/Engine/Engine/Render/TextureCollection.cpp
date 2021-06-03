#include "Engine/Config/Bootstrap.h"

#include "Engine/Engine/Render/TextureCollection.h"

#include "Engine/Debug/Log.h"
#include "Engine/Engine/Utils/ResourceLoader.h"
#include "Engine/Framework/Graphics/Device.h"
#include "Engine/Framework/Graphics/GraphicCommands.h"
#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/Memory/FrameworkMemoryInterface.h"
#include "Engine/Framework/String/String.h"
#include "Engine/Framework/Utils/CommandBuffer.h"

namespace hd
{
    namespace render
    {
        TextureCollection::TextureCollection(mem::AllocationScope& allocationScope, gfx::Device& device)
            : m_AllocationScope{ allocationScope }
            , m_Device{ device }
            , m_FirstTextureHolder{}
        {

        }

        TextureCollection::~TextureCollection()
        {
            for (TextureHolder* textureHolder = m_FirstTextureHolder; textureHolder != nullptr; textureHolder = textureHolder->Next)
            {
                m_Device.DestroyTexture(textureHolder->Handle);
            }
        }

        hd::gfx::TextureHandle TextureCollection::UploadTexture(char8_t const* textureFilePath, util::CommandBuffer& graphicsCommands)
        {
            mem::AllocationScope scratchScope{ mem::GetScratchAllocator() };
            str::String textureFilePathStr{ scratchScope, textureFilePath };

            TextureHolder* holder = m_FirstTextureHolder;
            while (holder)
            {
                if (holder->TextureKey == textureFilePathStr)
                {
                    return holder->Handle;
                }

                holder = holder->Next;
            }

            TextureHolder* newHolder = m_AllocationScope.AllocateObject<TextureHolder>(m_AllocationScope);
            newHolder->TextureKey.Assign(textureFilePathStr.CStr());

            mem::Buffer textureData{ scratchScope };
            util::ImageResource textureDesc{};
            util::LoadImage(scratchScope, textureFilePath, textureData, textureDesc);

            newHolder->Handle = m_Device.CreateTexture(textureDesc.Width, textureDesc.Height, 1, textureDesc.MipCount, textureDesc.Format, uint32_t(gfx::TextureFlags::ShaderResource),
                textureDesc.IsCube ? gfx::TextureDimenstion::TextureCube : gfx::TextureDimenstion::Texture2D, nullptr);

            gfx::GraphicCommandsStream commandStream{ graphicsCommands };
            commandStream.UpdateTexture(newHolder->Handle, 0, gfx::ALL_SUBRESOURCES, textureData.GetData(), textureData.GetSize());
            commandStream.UseAsReadableResource(newHolder->Handle);

            hdLogInfo(u8"Texture % loaded.", textureFilePathStr.CStr());

            newHolder->Next = m_FirstTextureHolder;
            m_FirstTextureHolder = newHolder;

            return newHolder->Handle;
        }

        TextureCollection::TextureHolder::TextureHolder(mem::AllocationScope& allocationScope)
            : TextureKey{ allocationScope }
            , Handle{}
            , Next{}
        {

        }
    }
}
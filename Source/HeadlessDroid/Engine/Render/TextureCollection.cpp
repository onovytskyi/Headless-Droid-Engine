#include "Config/Bootstrap.h"

#include "Engine/Render/TextureCollection.h"

#include "Debug/Log.h"
#include "Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Utils/ResourceLoader.h"
#include "Framework/Graphics/Device.h"
#include "Framework/Graphics/GraphicCommands.h"
#include "Framework/Memory/PlainDataArray.h"
#include "Framework/Utils/CommandBuffer.h"

namespace hd
{
	TextureCollection::TextureCollection(Device& device)
		: m_Device{ device }
		  , m_Textures{ &General() }
	{

	}

	TextureCollection::~TextureCollection()
	{
		for (auto& [textureName, textureHandle] : m_Textures)
		{
			m_Device.DestroyTexture(textureHandle);
		}
	}

	hd::TextureHandle TextureCollection::UploadTexture(char8_t const* textureFilePath, CommandBuffer& graphicsCommands)
	{
		ScopedScratchMemory scopedScratch{};

		Allocator& scratch = Scratch();

		std::pmr::u8string textureFilePathStr(textureFilePath, &Scratch());

		auto textureItem = m_Textures.find(textureFilePathStr);
		if (textureItem != m_Textures.end())
		{
			return textureItem->second;
		}

		PlainDataArray<std::byte> textureData{ &Scratch() };
		ImageResource textureDesc{};
		LoadImage(textureFilePath, textureData, textureDesc);

		TextureHandle handle = m_Device.CreateTexture(textureDesc.Width, textureDesc.Height, 1, textureDesc.MipCount, textureDesc.Format, TextureFlagsBits::ShaderResource,
		                                              textureDesc.IsCube ? TextureDimenstion::TextureCube : TextureDimenstion::Texture2D, nullptr);

		GraphicCommandsStream commandStream{ graphicsCommands };
		commandStream.UpdateTexture(handle, 0, ALL_SUBRESOURCES, textureData.Data(), textureData.Size());
		commandStream.UseAsReadableResource(handle);

		hdLogInfo(u8"Texture % loaded.", textureFilePathStr.c_str());

		m_Textures[textureFilePathStr] = handle;

		return handle;
	}
}

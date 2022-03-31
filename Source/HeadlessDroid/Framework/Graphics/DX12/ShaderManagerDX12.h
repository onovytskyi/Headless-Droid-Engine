#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Memory/PlainDataArray.h"
#include "Framework/Memory/VirtualLinearAllocator.h"
#include "Framework/Utils/Flags.h"

namespace hd
{
	class GfxBuffer;

    enum class ShaderFlagsBits
    {
        GenerateSymbols,
        IgnoreCache
    };
    using ShaderFlags = Flags<ShaderFlagsBits>;
    hdAllowFlagsForEnum(ShaderFlagsBits);

    class ShaderManager
    {
    public:
        ShaderManager();
        ~ShaderManager();

        PlainDataArray<std::byte> const& GetShader(char8_t const* shaderName, char8_t const* entryPoint, char8_t const* profile, ShaderFlags flags);

        void ResetShaderCache();

    private:
#if defined(HD_ENABLE_RESOURCE_COOKING)
        void CookShader(std::pmr::u8string const& shaderFilePath, std::pmr::u8string const& entryPoint, std::pmr::u8string const& profile, std::pmr::u8string const& cookedFilePath,
            ShaderFlags flags);
#endif

        VirtualLinearAllocator m_LocalAllocator;
        std::pmr::unordered_map<std::pmr::u8string, PlainDataArray<std::byte>> m_Shaders;
    };
}

#endif
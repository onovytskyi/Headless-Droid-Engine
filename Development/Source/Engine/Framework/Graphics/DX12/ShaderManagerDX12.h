#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Memory/Buffer.h"
#include "Engine/Framework/String/String.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
        class Buffer;
    }

    namespace gfx
    {
        class ShaderManager
        {
        public:
            ShaderManager(mem::AllocationScope& allocationScope);
            ~ShaderManager();

            mem::Buffer& GetShader(char8_t const* shaderName, char8_t const* entryPoint, char8_t const* profile);

        private:
            void LoadShaderMicrocode(mem::AllocationScope& scratch, str::String const& shaderName, str::String const& entryPoint, str::String const& profile, str::String const& cookedName, 
                mem::Buffer& output);

#if defined(HD_ENABLE_RESOURCE_COOKING)
            void CookShader(mem::AllocationScope& scratch, str::String const& shaderFilePath, str::String const& entryPoint, str::String const& profile, str::String const& cookedFilePath);
#endif

            struct ShaderHolder
            {
                ShaderHolder(mem::AllocationScope& allocationScope);

                str::String ShaderKey;
                mem::Buffer ShaderMicrocode;

                ShaderHolder* Next;
            };

            mem::AllocationScope& m_AllocationScope;
            ShaderHolder* m_FirstShaderHolder;
        };
    }
}

#endif
#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/DX12/BufferDX12.h"
#include "Engine/Framework/Graphics/DX12/TextureDX12.h"
#include "Engine/Framework/Graphics/GraphicsTypes.h"
#include "Engine/Framework/Utils/VirtualPoolAllocator.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace gfx
    {
        class Device;
        class ShaderManager;

        class BackendPlatform
        {
        public:
            BackendPlatform(mem::AllocationScope& allocationScope);
            ~BackendPlatform();

            ComPtr<IDXGIAdapter4> GetBestAdapter() const;
            IDXGIFactory6* GetNativeFactory() const;

            ShaderManager& GetShaderManager();
            util::VirtualPoolAllocator<Buffer>& GetBufferAllocator();
            util::VirtualPoolAllocator<Texture>& GetTextureAllocator();

        protected:
            ComPtr<IDXGIFactory6> m_Factory;
            ShaderManager* m_ShaderManager;

            util::VirtualPoolAllocator<Buffer> m_BufferAllocator;
            util::VirtualPoolAllocator<Texture> m_TextureAllocator;
        };
    }
}

#endif
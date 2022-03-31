#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Graphics/DX12/BufferDX12.h"
#include "Framework/Graphics/DX12/TextureDX12.h"
#include "Framework/Graphics/GraphicsTypes.h"
#include "Framework/Utils/VirtualPoolAllocator.h"

namespace hd
{
    class Allocator;
	class Device;
	class ShaderManager;

    class BackendPlatform
    {
    public:
        BackendPlatform(Allocator& persistentAllocator);
        ~BackendPlatform();

        ComPtr<IDXGIAdapter4> GetBestAdapter() const;
        IDXGIFactory6* GetNativeFactory() const;

        ShaderManager& GetShaderManager();
        VirtualPoolAllocator<GfxBuffer>& GetBufferAllocator();
        VirtualPoolAllocator<Texture>& GetTextureAllocator();

    protected:
        Allocator& m_PersistentAllocator;

        ComPtr<IDXGIFactory6> m_Factory;
        ShaderManager* m_ShaderManager;

        VirtualPoolAllocator<GfxBuffer> m_BufferAllocator;
        VirtualPoolAllocator<Texture> m_TextureAllocator;
    };
}

#endif
#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace gfx
    {
        class Backend;

        class DevicePlatform
        {
        public:
            DevicePlatform(Backend& backend);
            ~DevicePlatform();

            ID3D12Device* GetNativeDevice() const;

            TextureHandle RegisterTexture(ID3D12Resource* resource, D3D12_RESOURCE_STATES state, GraphicFormat format, uint32_t flags);

        protected:
            Backend* m_Backend;

            ComPtr<IDXGIAdapter4> m_Adapter;
            ComPtr<ID3D12Device> m_Device;
#if defined(HD_ENABLE_GFX_DEBUG)
            DWORD m_MessageCallbackCookie;
#endif
        };
    }
}

#endif
#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/TextureDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

namespace hd
{
    namespace gfx
    {
        Texture::Texture(ID3D12Resource* resource)
            : m_Resource{ resource }
        {

        }

        ID3D12Resource* Texture::GetNativeResource() const
        {
            return m_Resource;
        }
    }
}

#endif
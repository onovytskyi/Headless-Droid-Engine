#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/TextureDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

namespace hd
{
    namespace gfx
    {
        Texture::Texture(ID3D12Resource* resource)
            : Texture(resource, D3D12_RESOURCE_STATE_COMMON)
        {
        }

        Texture::Texture(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState)
            : m_Data{}
        {
            m_Data.Resource = resource;
            m_Data.State = initialState;
        }

        ID3D12Resource* Texture::GetNativeResource() const
        {
            return m_Data.Resource;
        }

        ResourceStateTracker::StateTrackedData& Texture::GetStateTrackedData()
        {
            return m_Data;
        }
    }
}

#endif
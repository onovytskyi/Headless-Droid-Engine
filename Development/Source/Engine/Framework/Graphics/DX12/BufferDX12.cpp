#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/BufferDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

namespace hd
{
    namespace gfx
    {
        Buffer::Buffer(ID3D12Resource* resource)
            : Buffer(resource, D3D12_RESOURCE_STATE_COMMON)
        {
        }

        Buffer::Buffer(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState)
            : m_Data{}
        {
            m_Data.Resource = resource;
            m_Data.State = initialState;
        }

        ID3D12Resource* Buffer::GetNativeResource() const
        {
            return m_Data.Resource;
        }

        ResourceStateTracker::StateTrackedData& Buffer::GetStateTrackedData()
        {
            return m_Data;
        }
    }
}

#endif
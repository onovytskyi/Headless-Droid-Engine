#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/BufferDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/Device.h"

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

        void Buffer::Free(Device& device)
        {
            m_Data.Resource->Release();
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
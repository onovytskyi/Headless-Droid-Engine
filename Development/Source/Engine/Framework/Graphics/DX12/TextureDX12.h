#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/DX12/DescriptorManagerDX12.h"
#include "Engine/Framework/Graphics/DX12/ResourceStateTrackerDX12.h"
#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace gfx
    {
        class Device;

        class Texture
        {
        public:
            Texture(Device& device, ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState, GraphicFormat format, uint32_t flags);

            hdNoncopyable(Texture)

            void Free(Device& device);

            ID3D12Resource* GetNativeResource() const;
            ResourceStateTracker::StateTrackedData& GetStateTrackedData();

            DescriptorRTV GetRTV() const;
            DescriptorDSV GetDSV() const;
            DescriptorSRV GetSRV() const;
            DescriptorSRV GetUAV() const;

            DescriptorSRV GetOrCreateSubresoruceSRV(Device& device, uint32_t subresourceIdx);
            DescriptorSRV GetOrCreateSubresoruceUAV(Device& device, uint32_t subresourceIdx);

        private:
            void CreateViews(Device& device, GraphicFormat format, uint32_t flags, D3D12_RESOURCE_DIMENSION dimension, bool isCube);

            ResourceStateTracker::StateTrackedData m_Data;
            GraphicFormat m_Format;
            DescriptorRTV m_RTV;
            DescriptorDSV m_DSV;
            DescriptorSRV m_SRV;
            DescriptorSRV m_UAV;

            DescriptorSRV m_SubresourceSRV[ResourceStateTracker::StateTrackedData::MAX_SUBRESOURCES];
            DescriptorSRV m_SubresourceUAV[ResourceStateTracker::StateTrackedData::MAX_SUBRESOURCES];
        };
    }
}

#endif
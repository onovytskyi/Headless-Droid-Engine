#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/DX12/DescriptorManagerDX12.h"
#include "Engine/Framework/Graphics/DX12/HeapAllocatorDX12.h"
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
            Texture(Device& device, ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState, GraphicFormat format, TextureFlags flags, TextureDimenstion dimension);
            Texture(Device& device, ID3D12Resource* resource, HeapAllocator::Allocation const& heapAllocation, D3D12_RESOURCE_STATES initialState, GraphicFormat format, TextureFlags flags, 
                TextureDimenstion dimension);

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

            uint64_t GetWidth() const;
            uint32_t GetHeight() const;

            uint32_t GetSubresourceCount() const;
            GraphicFormat GetFormat() const;

        private:
            void CreateViews(Device& device, GraphicFormat format, TextureFlags flags, TextureDimenstion dimension);
            void UpdateDataFromNativeDescription();

            ResourceStateTracker::StateTrackedData m_Data;
            HeapAllocator::Allocation m_HeapAllocation;

            GraphicFormat m_Format;
            TextureDimenstion m_Dimension;

            DescriptorRTV m_RTV;
            DescriptorDSV m_DSV;
            DescriptorSRV m_SRV;
            DescriptorSRV m_UAV;

            DescriptorSRV m_SubresourceSRV[ResourceStateTracker::StateTrackedData::MAX_SUBRESOURCES];
            DescriptorSRV m_SubresourceUAV[ResourceStateTracker::StateTrackedData::MAX_SUBRESOURCES];

            uint32_t m_SubresourceCount;
            uint64_t m_Width;
            uint32_t m_Height;
        };
    }
}

#endif
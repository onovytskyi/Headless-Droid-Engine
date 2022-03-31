#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Graphics/DX12/DescriptorManagerDX12.h"
#include "Framework/Graphics/DX12/HeapAllocatorDX12.h"
#include "Framework/Graphics/DX12/ResourceStateTrackerDX12.h"
#include "Framework/Graphics/GraphicsTypes.h"

namespace hd
{
	class Device;

	class GfxBuffer
	{
	public:
		GfxBuffer(Device& device, ID3D12Resource* resource, uint32_t numElements, uint32_t elementSize, BufferFlags flags);
		GfxBuffer(Device& device, ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState, uint32_t numElements, uint32_t elementSize, BufferFlags flags);
		GfxBuffer(Device& device, HeapAllocator::Allocation const& heapAllocation, uint32_t numElements, uint32_t elementSize, BufferFlags flags);

		hdNoncopyable(GfxBuffer)

		void Free(Device& device);

		ID3D12Resource* GetNativeResource() const;
		ResourceStateTracker::StateTrackedData& GetStateTrackedData();

		DescriptorSRV GetCBV() const;
		DescriptorSRV GetSRV() const;
		DescriptorSRV GetUAV() const;

		size_t GetBaseOffset() const;
		uint32_t GetSize() const;

	private:
		void CreateViews(Device& device, uint32_t numElements, uint32_t elementSize, BufferFlags flags);

		ResourceStateTracker::StateTrackedData m_Data;
		HeapAllocator::Allocation m_HeapAllocation;

		DescriptorSRV m_CBV;
		DescriptorSRV m_SRV;
		DescriptorSRV m_UAV;

		uint32_t m_Size;
	};
}

#endif

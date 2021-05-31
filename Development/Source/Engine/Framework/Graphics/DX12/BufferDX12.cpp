#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/BufferDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/Device.h"

namespace hd
{
    namespace gfx
    {
        Buffer::Buffer(Device& device, ID3D12Resource* resource, uint32_t numElements, uint32_t elementSize, uint32_t flags)
            : Buffer(device, resource, D3D12_RESOURCE_STATE_COMMON, numElements, elementSize, flags)
        {
        }

        Buffer::Buffer(Device& device, ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState, uint32_t numElements, uint32_t elementSize, uint32_t flags)
            : m_Data{}
            , m_HeapAllocation{}
            , m_CBV{}
            , m_SRV{}
            , m_UAV{}
            , m_Size{}
        {
            m_Data.Resource = resource;
            m_Data.State = initialState;

            m_Size = uint32_t(m_Data.Resource->GetDesc().Width);

            CreateViews(device, numElements, elementSize, flags);
        }

        Buffer::Buffer(Device& device, HeapAllocator::Allocation const& heapAllocation, uint32_t numElements, uint32_t elementSize, uint32_t flags)
            : m_Data{}
            , m_HeapAllocation{ heapAllocation }
            , m_CBV{}
            , m_SRV{}
            , m_UAV{}
            , m_Size{ uint32_t(heapAllocation.Size) }
        {
            CreateViews(device, numElements, elementSize, flags);
        }

        void Buffer::Free(Device& device)
        {
            if (m_Data.Resource)
            {
                m_Data.Resource->Release();
            }

            if (m_HeapAllocation.IsValid())
            {
                device.GetHeapAllocator().Free(m_HeapAllocation);
            }

            DescriptorManager& descriptorManager = device.GetDescriptorManager();

            if (m_CBV)
            {
                descriptorManager.Free(m_CBV);
            }

            if (m_SRV)
            {
                descriptorManager.Free(m_SRV);
            }

            if (m_UAV)
            {
                descriptorManager.Free(m_UAV);
            }
        }

        ID3D12Resource* Buffer::GetNativeResource() const
        {
            return m_Data.Resource != nullptr ? m_Data.Resource : m_HeapAllocation.ResourceData->Resource;
        }

        ResourceStateTracker::StateTrackedData& Buffer::GetStateTrackedData()
        {
            return m_Data.Resource != nullptr ? m_Data : *m_HeapAllocation.ResourceData;
        }

        DescriptorSRV Buffer::GetCBV() const
        {
            return m_CBV;
        }

        DescriptorSRV Buffer::GetSRV() const
        {
            return m_SRV;
        }

        DescriptorSRV Buffer::GetUAV() const
        {
            return m_UAV;
        }

        size_t Buffer::GetBaseOffset() const
        {
            return m_Data.Resource != nullptr ? 0 : m_HeapAllocation.Offset;
        }

        uint32_t Buffer::GetSize() const
        {
            return m_Size;
        }

        void Buffer::CreateViews(Device& device, uint32_t numElements, uint32_t elementSize, uint32_t flags)
        {
            DescriptorManager& descriptorManager = device.GetDescriptorManager();

            if (flags & uint32_t(BufferFlags::ConstantBuffer))
            {
                m_CBV = descriptorManager.AllocateSRV();

                D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
                cbvDesc.BufferLocation = GetNativeResource()->GetGPUVirtualAddress() + GetBaseOffset();
                cbvDesc.SizeInBytes = m_Size;

                device.GetNativeDevice()->CreateConstantBufferView(&cbvDesc, m_CBV.HandleCPU);
            }

            if (flags & uint32_t(BufferFlags::ShaderResource))
            {
                m_SRV = descriptorManager.AllocateSRV();

                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
                srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                srvDesc.Format = DXGI_FORMAT_UNKNOWN;
                srvDesc.Buffer.FirstElement = GetBaseOffset() / elementSize;
                srvDesc.Buffer.NumElements = numElements;
                srvDesc.Buffer.StructureByteStride = elementSize;
                srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

                device.GetNativeDevice()->CreateShaderResourceView(GetNativeResource(), &srvDesc, m_SRV.HandleCPU);
            }

            if (flags & uint32_t(BufferFlags::UnorderedAccess))
            {
                m_UAV = descriptorManager.AllocateSRV();

                D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
                uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
                uavDesc.Format = DXGI_FORMAT_UNKNOWN;
                uavDesc.Buffer.FirstElement = GetBaseOffset() / elementSize;
                uavDesc.Buffer.NumElements = numElements;
                uavDesc.Buffer.StructureByteStride = elementSize;
                uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

                device.GetNativeDevice()->CreateUnorderedAccessView(GetNativeResource(), nullptr, &uavDesc, m_UAV.HandleCPU);
            }
        }
    }
}

#endif
#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/DescriptorManagerDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Debug/Assert.h"
#include "Engine/Framework/Graphics/Device.h"

namespace hd
{
    namespace gfx
    {
        DescriptorManager::DescriptorManager(DevicePlatform& device, mem::AllocationScope& allocationScope)
            : m_RTV{ device, allocationScope, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, cfg::MaxDescriptorsRTV() }
            , m_DSV{ device, allocationScope, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, cfg::MaxDescriptorsDSV() }
            , m_SRV{ device, allocationScope, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cfg::MaxDescriptorsSRV() }
            , m_Sampler{ device, allocationScope, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, cfg::MaxDescriptorsSampler() }
        {

        }

        DescriptorManager::~DescriptorManager()
        {

        }

        DescriptorRTV DescriptorManager::AllocateRTV()
        {
            uint32_t descriptorIdx = m_RTV.Allocate();

            DescriptorRTV descriptor{};
            descriptor.HandleCPU = m_RTV.ResolveCPU(descriptorIdx);

            return descriptor;
        }

        DescriptorDSV DescriptorManager::AllocateDSV()
        {
            uint32_t descriptorIdx = m_DSV.Allocate();

            DescriptorDSV descriptor{};
            descriptor.HandleCPU = m_DSV.ResolveCPU(descriptorIdx);

            return descriptor;
        }

        DescriptorSRV DescriptorManager::AllocateSRV()
        {
            uint32_t descriptorIdx = m_SRV.Allocate();

            DescriptorSRV descriptor{};
            descriptor.HeapIndex = descriptorIdx;
            descriptor.HandleCPU = m_SRV.ResolveCPU(descriptorIdx);
            descriptor.HandleGPU = m_SRV.ResolveGPU(descriptorIdx);

            return descriptor;
        }

        DescriptorSampler DescriptorManager::AllocateSampler()
        {
            uint32_t descriptorIdx = m_Sampler.Allocate();

            DescriptorSampler descriptor{};
            descriptor.HeapIndex = descriptorIdx;
            descriptor.HandleCPU = m_Sampler.ResolveCPU(descriptorIdx);
            descriptor.HandleGPU = m_Sampler.ResolveGPU(descriptorIdx);

            return descriptor;
        }

        void DescriptorManager::Free(DescriptorRTV descriptor)
        {
            m_RTV.Free(descriptor.HandleCPU);
        }

        void DescriptorManager::Free(DescriptorDSV descriptor)
        {
            m_DSV.Free(descriptor.HandleCPU);
        }

        void DescriptorManager::Free(DescriptorSRV descriptor)
        {
            m_SRV.Free(descriptor.HandleCPU);
        }

        void DescriptorManager::Free(DescriptorSampler descriptor)
        {
            m_Sampler.Free(descriptor.HandleCPU);
        }

        DescriptorManager::DescriptorAllocator::DescriptorAllocator(DevicePlatform& device, mem::AllocationScope& allocationScope, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t maxDescriptors)
            : m_Allocator{ allocationScope, maxDescriptors }
            , m_HandleIncrementSize{}
        {
            m_HandleIncrementSize = device.GetNativeDevice()->GetDescriptorHandleIncrementSize(type);

            D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
            heapDesc.Type = type;
            heapDesc.Flags = (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
            heapDesc.NumDescriptors = maxDescriptors;

            hdEnsure(device.GetNativeDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_Heap.GetAddressOf())));
        }

        uint32_t DescriptorManager::DescriptorAllocator::Allocate()
        {
            return uint32_t(m_Allocator.Allocate(1, 1));
        }

        void DescriptorManager::DescriptorAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE handleCPU)
        {
            size_t index = (handleCPU.ptr - m_Heap->GetCPUDescriptorHandleForHeapStart().ptr) / m_HandleIncrementSize;
            m_Allocator.Deallocate(index, 1);
        }

        ID3D12DescriptorHeap* DescriptorManager::DescriptorAllocator::GetDescriptorHeap()
        {
            return m_Heap.Get();
        }

        D3D12_CPU_DESCRIPTOR_HANDLE DescriptorManager::DescriptorAllocator::ResolveCPU(uint32_t index)
        {
            hdAssert(index < m_Heap->GetDesc().NumDescriptors, u8"Descriptor index is out of range.");

            D3D12_CPU_DESCRIPTOR_HANDLE handle{};
            handle.ptr = m_Heap->GetCPUDescriptorHandleForHeapStart().ptr + size_t(m_HandleIncrementSize) * index;

            return handle;
        }

        D3D12_GPU_DESCRIPTOR_HANDLE DescriptorManager::DescriptorAllocator::ResolveGPU(uint32_t index)
        {
            hdAssert(index < m_Heap->GetDesc().NumDescriptors, u8"Descriptor index is out of range.");
            hdAssert(m_Heap->GetDesc().Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV || m_Heap->GetDesc().Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 
                u8"GPU visible descriptors for this type are not supported.");

            D3D12_GPU_DESCRIPTOR_HANDLE handle{};
            handle.ptr = m_Heap->GetGPUDescriptorHandleForHeapStart().ptr + size_t(m_HandleIncrementSize) * index;

            return handle;
        }
    }
}

#endif
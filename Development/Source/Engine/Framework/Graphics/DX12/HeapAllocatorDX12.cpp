#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/HeapAllocatorDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/Device.h"

namespace hd
{
    namespace gfx
    {

        HeapAllocator::HeapAllocator(Device& device, mem::AllocationScope& allocationScope, size_t heapSize, size_t maxHeaps, size_t keepAliveFrames)
            : m_OwnerDevice{ device }
            , m_AllocationScope{ allocationScope }
            , m_HeapSize{ heapSize }
            , m_KeepAliveFrames{ keepAliveFrames }
            , m_Heaps{ allocationScope, maxHeaps }
            , m_PendingHeaps{ allocationScope, maxHeaps }
            , m_FreeHeaps{ allocationScope, maxHeaps }
            , m_NonresidentHeaps{ allocationScope, maxHeaps }
        {

        }

        HeapAllocator::~HeapAllocator()
        {
            for (Heap* heap : m_Heaps)
            {
                heap->Destroy();
            }

            for (Heap* heap : m_PendingHeaps)
            {
                heap->Destroy();
            }

            for (Heap* heap : m_FreeHeaps)
            {
                heap->Destroy();
            }
        }

        HeapAllocator::Allocation HeapAllocator::Allocate(size_t size, size_t alignment, D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags, Usage usage)
        {
            hdAssert(size <= m_HeapSize);

            for (Heap* heap : m_Heaps)
            {
                if (heap->IsCompatible(type, flags, usage))
                {
                    Allocation allocation = heap->Allocate(size, alignment);
                    if (allocation.Offset != util::BestFitAllocatorHelper::INVALID_INDEX)
                    {
                        return allocation;
                    }
                }
            }

            for (size_t heapIdx = 0; heapIdx < m_FreeHeaps.GetSize(); ++heapIdx)
            {
                if (m_FreeHeaps[heapIdx]->IsCompatible(type, flags))
                {
                    Heap* heapToUse = m_FreeHeaps[heapIdx];
                    Allocation allocation = heapToUse->Allocate(size, alignment);

                    if (allocation.Offset != util::BestFitAllocatorHelper::INVALID_INDEX)
                    {
                        m_FreeHeaps.RemoveFast(heapIdx);

                        heapToUse->ChangeUsage(usage);
                        m_Heaps.Add(heapToUse);

                        return allocation;
                    }
                }
            }

            if (m_NonresidentHeaps.GetSize() != 0)
            {
                Heap* heapToUse = m_NonresidentHeaps[0];
                m_NonresidentHeaps.RemoveFast(0);

                heapToUse->Create(m_OwnerDevice, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, type, flags, usage);
                m_Heaps.Add(heapToUse);

                return heapToUse->Allocate(size, alignment);
            }

            Heap* newHeap = m_AllocationScope.AllocateObject<Heap>(m_AllocationScope, m_HeapSize);
            newHeap->Create(m_OwnerDevice, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, type, flags, usage);
            m_Heaps.Add(newHeap);

            return newHeap->Allocate(size, alignment);
        }

        void HeapAllocator::Free(Allocation& allocation)
        {
            for (Heap* heap : m_Heaps)
            {
                if (heap->GetNativeHeap() == allocation.GPUHeap)
                {
                    heap->Free(allocation);
                }
            }
        }

        void HeapAllocator::RecycleResources(uint64_t currentMarker, uint64_t completedMarker)
        {
            // Check if we can make some of the heaps nonresident
            for (size_t heapIdx = 0; heapIdx < m_FreeHeaps.GetSize(); ++heapIdx)
            {
                Heap* heap = m_FreeHeaps[heapIdx];
                if ((completedMarker - heap->GetMarker()) >= m_KeepAliveFrames)
                {
                    m_FreeHeaps.RemoveFast(heapIdx);
                    heapIdx -= 1;

                    heap->Destroy();
                    m_NonresidentHeaps.Add(heap);
                }
            }

            // Check if we can make transient heaps free
            for (size_t heapIdx = 0; heapIdx < m_PendingHeaps.GetSize(); ++heapIdx)
            {
                Heap* heap = m_PendingHeaps[heapIdx];
                if (heap->GetMarker() <= completedMarker)
                {
                    m_PendingHeaps.RemoveFast(heapIdx);
                    heapIdx -= 1;

                    m_FreeHeaps.Add(heap);
                }
            }

            // Check if we can free some of the heaps
            for (size_t heapIdx = 0; heapIdx < m_Heaps.GetSize(); ++heapIdx)
            {
                Heap* heap = m_Heaps[heapIdx];
                if (heap->IsEmpty())
                {
                    m_Heaps.RemoveFast(heapIdx);
                    heapIdx -= 1;

                    heap->SetMarker(currentMarker);

                    if (heap->GetUsage() == Usage::Transient)
                    {
                        m_PendingHeaps.Add(heap);
                    }
                    else
                    {
                        m_FreeHeaps.Add(heap);
                    }
                }
            }
        }

        HeapAllocator::Heap::Heap(mem::AllocationScope& allocationScope, size_t size)
            : m_Marker{ std::numeric_limits<size_t>::max() }
            , m_Type{ D3D12_HEAP_TYPE_CUSTOM }
            , m_Flags{ D3D12_HEAP_FLAG_NONE }
            , m_Usage{ Usage::Persistent }
            , m_Allocator{ allocationScope, size }
            , m_NumTransientAllocations{}
            , m_Heap{}
            , m_ResourceData{}
            , m_CPUMappedData{}
        {

        }

        void HeapAllocator::Heap::ChangeUsage(Usage usage)
        {
            hdAssert(IsCreated(), u8"Cannot use nonresident heap.");

            m_Usage = usage;
        }

        HeapAllocator::Usage HeapAllocator::Heap::GetUsage() const
        {
            return m_Usage;
        }

        bool HeapAllocator::Heap::IsCompatible(D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags, Usage usage)
        {
            hdAssert(IsCreated(), u8"Cannot use nonresident heap.");

            return m_Type == type && m_Flags == flags && m_Usage == usage;
        }

        bool HeapAllocator::Heap::IsCompatible(D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags)
        {
            hdAssert(IsCreated(), u8"Cannot use nonresident heap.");

            return m_Type == type && m_Flags == flags;
        }

        HeapAllocator::Allocation HeapAllocator::Heap::Allocate(size_t size, size_t alignment)
        {
            hdAssert(IsCreated(), u8"Cannot use nonresident heap.");

            HeapAllocator::Allocation allocation{};
            allocation.Offset = m_Allocator.Allocate(size, alignment);
            allocation.Size = size;
            allocation.CPUMappedMemory = reinterpret_cast<std::byte*>(m_CPUMappedData) + allocation.Offset;
            allocation.GPUHeap = m_Heap;
            allocation.ResourceData = &m_ResourceData;

            if (m_Usage == Usage::Transient)
            {
                m_NumTransientAllocations += 1;
            }

            return allocation;
        }

        void HeapAllocator::Heap::Free(Allocation& allocation)
        {
            hdAssert(IsCreated(), u8"Cannot use nonresident heap.");

            if (m_Usage == Usage::Persistent)
            {
                m_Allocator.Deallocate(allocation.Offset, allocation.Size);
            }
            else
            {
                hdAssert(m_NumTransientAllocations > 0, u8"Trying to deallocate from free heap.");
                m_NumTransientAllocations -= 1;
            }
        }

        ID3D12Heap* HeapAllocator::Heap::GetNativeHeap()
        {
            hdAssert(IsCreated(), u8"Cannot use nonresident heap.");

            return m_Heap;
        }

        void HeapAllocator::Heap::Create(Device& device, size_t alignment, D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags, Usage usage)
        {
            D3D12_HEAP_DESC heapDesc{};
            heapDesc.SizeInBytes = m_Allocator.GetSize();
            heapDesc.Alignment = alignment;
            heapDesc.Properties.Type = type;
            heapDesc.Flags = flags;

            hdEnsure(device.GetNativeDevice()->CreateHeap(&heapDesc, IID_PPV_ARGS(&m_Heap)));

            D3D12_RESOURCE_DESC resourceDesc{};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            resourceDesc.Width = heapDesc.SizeInBytes;
            resourceDesc.Height = 1;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

            if (!(flags & D3D12_HEAP_FLAG_DENY_BUFFERS))
            {
                m_ResourceData.State = D3D12_RESOURCE_STATE_COMMON;
                if (type == D3D12_HEAP_TYPE_UPLOAD)
                {
                    m_ResourceData.State = D3D12_RESOURCE_STATE_GENERIC_READ;
                }
                else if (type == D3D12_HEAP_TYPE_READBACK)
                {
                    m_ResourceData.State = D3D12_RESOURCE_STATE_COPY_DEST;
                }

                hdEnsure(device.GetNativeDevice()->CreatePlacedResource(m_Heap, 0, &resourceDesc, m_ResourceData.State, nullptr, IID_PPV_ARGS(&m_ResourceData.Resource)));

                if (type == D3D12_HEAP_TYPE_UPLOAD)
                {
                    m_ResourceData.Resource->Map(0, nullptr, &m_CPUMappedData);
                }
            }

            m_Type = type;
            m_Flags = flags;
            m_Usage = usage;
        }

        void HeapAllocator::Heap::Destroy()
        {
            hdAssert(IsCreated(), u8"Cannot destroy nonresident heap.");

            if (m_ResourceData.Resource)
            {
                m_ResourceData.Resource->Release();
            }

            m_Heap->Release();
            m_Heap = nullptr;
        }

        bool HeapAllocator::Heap::IsCreated()
        {
            return m_Heap != nullptr;
        }

        bool HeapAllocator::Heap::IsEmpty()
        {
            return m_Usage == Usage::Persistent ? m_Allocator.Empty() : m_NumTransientAllocations == 0;
        }

        void HeapAllocator::Heap::SetMarker(size_t marker)
        {
            m_Marker = marker;
        }

        size_t HeapAllocator::Heap::GetMarker()
        {
            return m_Marker;
        }

        bool HeapAllocator::Allocation::IsValid()
        {
            return GPUHeap != nullptr;
        }
    }
}

#endif
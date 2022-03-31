#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Graphics/DX12/ResourceStateTrackerDX12.h"
#include "Framework/Utils/BestFitAllocatorHelper.h"

namespace hd
{
    class Allocator;

    namespace gfx
    {
        class Device;

        class HeapAllocator
        {
        public:
            enum class Usage
            {
                Persistent,
                Transient
            };

            struct Allocation
            {
                bool IsValid();

                size_t Offset;
                size_t Size;
                void* CPUMappedMemory;
                ID3D12Heap* GPUHeap;
                ResourceStateTracker::StateTrackedData* ResourceData;
            };

            HeapAllocator(Allocator& generalAllocator, Device& device, size_t heapSize, size_t keepAliveFrames);
            ~HeapAllocator();

            Allocation Allocate(size_t size, size_t alignment, D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags, Usage usage);
            void Free(Allocation& allocation);
            void RecycleResources(uint64_t currentMarker, uint64_t completedMarker);

        private:
            class Heap
            {
            public:
                Heap(Allocator& generalAllocator, size_t size);

                void ChangeUsage(Usage usage);
                Usage GetUsage() const;

                bool IsCompatible(D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags, Usage usage);
                bool IsCompatible(D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags);

                Allocation Allocate(size_t size, size_t alignment);
                void Free(Allocation& allocation);

                ID3D12Heap* GetNativeHeap();

                void Create(Device& device, size_t alignment, D3D12_HEAP_TYPE type, D3D12_HEAP_FLAGS flags, Usage usage);
                void Destroy();

                bool IsCreated();
                bool IsEmpty();

                void SetMarker(size_t marker);
                size_t GetMarker();

                void Reset();

            private:
                size_t m_Marker;

                D3D12_HEAP_TYPE m_Type;
                D3D12_HEAP_FLAGS m_Flags;
                Usage m_Usage;

                // #TODO Use linear allocator and twice as smaller heaps for Transient allocations
                util::BestFitAllocatorHelper m_Allocator;
                uint32_t m_NumTransientAllocations;

                ID3D12Heap* m_Heap;
                ResourceStateTracker::StateTrackedData m_ResourceData;
                void* m_CPUMappedData;
            };

            Allocator& m_GeneralAllocator;

            Device& m_OwnerDevice;
            size_t m_HeapSize;
            size_t m_KeepAliveFrames;
            std::pmr::vector<Heap*> m_Heaps;
            std::pmr::vector<Heap*> m_PendingHeaps;
            std::pmr::vector<Heap*> m_FreeHeaps;
            std::pmr::vector<Heap*> m_NonresidentHeaps;
        };
    }
}

#endif
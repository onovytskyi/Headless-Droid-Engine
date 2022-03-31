#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Utils/BestFitAllocatorHelper.h"

namespace hd
{
    class Allocator;

    namespace gfx
    {
        class DevicePlatform;

        struct DescriptorRTV
        {
            D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;

            operator bool() const { return HandleCPU.ptr != 0; }
        };

        struct DescriptorDSV
        {
            D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;

            operator bool() const { return HandleCPU.ptr != 0; }
        };

        struct DescriptorSRV
        {
            uint32_t HeapIndex;
            D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;
            D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;

            operator bool() const { return HandleCPU.ptr != 0; }
        };

        struct DescriptorSampler
        {
            uint32_t HeapIndex;
            D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;
            D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;

            operator bool() const { return HandleCPU.ptr != 0; }
        };

        class DescriptorManager
        {
        public:
            DescriptorManager(Allocator& generalAllocator, DevicePlatform& device);
            ~DescriptorManager();

            DescriptorRTV AllocateRTV();
            DescriptorDSV AllocateDSV();
            DescriptorSRV AllocateSRV();
            DescriptorSampler AllocateSampler();

            void Free(DescriptorRTV descriptor);
            void Free(DescriptorDSV descriptor);
            void Free(DescriptorSRV descriptor);
            void Free(DescriptorSampler descriptor);

            ID3D12DescriptorHeap* GetResourceHeap();
            ID3D12DescriptorHeap* GetSamplerHeap();

        private:
            class DescriptorAllocator
            {
            public:
                DescriptorAllocator(Allocator& generalAllocator, DevicePlatform& device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t maxDescriptors);

                uint32_t Allocate();
                void Free(D3D12_CPU_DESCRIPTOR_HANDLE handleCPU);

                ID3D12DescriptorHeap* GetDescriptorHeap();
                D3D12_CPU_DESCRIPTOR_HANDLE ResolveCPU(uint32_t index);
                D3D12_GPU_DESCRIPTOR_HANDLE ResolveGPU(uint32_t index);

                void FillWithDefaults(DevicePlatform& device, ID3D12Resource* defaultResource);

            private:
                util::BestFitAllocatorHelper m_Allocator;
                ComPtr<ID3D12DescriptorHeap> m_Heap;
                uint32_t m_HandleIncrementSize;
            };

            DescriptorAllocator m_RTV;
            DescriptorAllocator m_DSV;
            DescriptorAllocator m_SRV;
            DescriptorAllocator m_Sampler;

            ComPtr<ID3D12Resource> m_DefaultDescriptorResource;
        };
    }
}

#endif
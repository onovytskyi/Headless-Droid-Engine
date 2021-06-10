#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/DX12/CommandListManagerDX12.h"
#include "Engine/Framework/Graphics/DX12/DescriptorManagerDX12.h"
#include "Engine/Framework/Graphics/DX12/HeapAllocatorDX12.h"
#include "Engine/Framework/Graphics/DX12/ResourceStateTrackerDX12.h"
#include "Engine/Framework/Graphics/GraphicsTypes.h"
#include "Engine/Framework/Utils/BufferArray.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace util
    {
        class CommandBuffer;
    }

    namespace gfx
    {
        class Backend;
        class Queue;
        class RenderStatePlatform;

        class DevicePlatform
        {
        public:
            DevicePlatform(Backend& backend, mem::AllocationScope& allocationScope);
            ~DevicePlatform();

            ID3D12Device2* GetNativeDevice() const;
            ID3D12RootSignature* GetNativeRootSignature() const;

            TextureHandle RegisterTexture(ID3D12Resource* resource, D3D12_RESOURCE_STATES state, GraphicFormat format, TextureFlags flags, TextureDimenstion dimension);

            void PresentOnQueue(Queue& queue, TextureHandle framebuffer);
            void SubmitToQueue(Queue& queue, util::CommandBuffer& commandBuffer);

            DescriptorManager& GetDescriptorManager();
            HeapAllocator& GetHeapAllocator();

#if defined(HD_ENABLE_RESOURCE_COOKING)
            void RegisterRenderStateForRebuild(RenderStatePlatform* renderState);
            void UnregisterRenderStateForRebuild(RenderStatePlatform* renderState);
#endif

        protected:
            void CreateUnifiedRootSignature();

            Backend* m_Backend;

            ComPtr<IDXGIAdapter4> m_Adapter;
            ComPtr<ID3D12Device2> m_Device;
            ComPtr<ID3D12RootSignature> m_RootSignature;
#if defined(HD_ENABLE_GFX_DEBUG)
            DWORD m_MessageCallbackCookie;
#endif
            CommandListManager* m_GraphicsCommandListManager;
            CommandListManager* m_ComputeCommandListManager;
            CommandListManager* m_CopyCommandListManager;

            ResourceStateTracker* m_ResourceStateTracker;
            DescriptorManager* m_DescriptorManager;

            HeapAllocator* m_HeapAllocator;

            template<typename T>
            struct ResourceHolder
            {
                size_t FrameMarker;
                T Resource;
            };

            util::BufferArray<ResourceHolder<BufferHandle>> m_BuffersToFree;
            util::BufferArray<ResourceHolder<TextureHandle>> m_TexturesToFree;

            util::BufferArray<BufferHandle> m_RecentBuffersToFree;
            util::BufferArray<TextureHandle> m_RecentTexturesToFree;

#if defined(HD_ENABLE_RESOURCE_COOKING)
            util::BufferArray<RenderStatePlatform*> m_RenderStatesToRebuild;
#endif
        };
    }
}

#endif
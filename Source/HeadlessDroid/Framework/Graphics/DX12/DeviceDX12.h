#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Graphics/DX12/CommandListManagerDX12.h"
#include "Framework/Graphics/DX12/DescriptorManagerDX12.h"
#include "Framework/Graphics/DX12/HeapAllocatorDX12.h"
#include "Framework/Graphics/DX12/ResourceStateTrackerDX12.h"
#include "Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    class Allocator;

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
            DevicePlatform(Allocator& persistentAllocator, Allocator& generalAllocator, Backend& backend);
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

            Allocator& m_PersistentAllocator;
            Allocator& m_GeneralAllocator;

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

            std::pmr::vector<ResourceHolder<BufferHandle>> m_BuffersToFree;
            std::pmr::vector<ResourceHolder<TextureHandle>> m_TexturesToFree;

            std::pmr::vector<BufferHandle> m_RecentBuffersToFree;
            std::pmr::vector<TextureHandle> m_RecentTexturesToFree;

#if defined(HD_ENABLE_RESOURCE_COOKING)
            std::pmr::vector<RenderStatePlatform*> m_RenderStatesToRebuild;
#endif
        };
    }
}

#endif
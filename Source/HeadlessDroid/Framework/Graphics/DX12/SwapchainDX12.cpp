#include "Config/Bootstrap.h"

#include "Framework/Graphics/Swapchain.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Debug/Assert.h"
#include "Debug/Log.h"
#include "Foundation/Memory/Utils.h"
#include "Framework/Graphics/Backend.h"
#include "Framework/Graphics/DX12/TextureDX12.h"
#include "Framework/Graphics/DX12/UtilsDX12.h"
#include "Framework/Graphics/Device.h"
#include "Framework/Graphics/Fence.h"
#include "Framework/Graphics/Queue.h"
#include "Framework/System/SystemWindow.h"

namespace hd
{
    namespace gfx
    {
        SwapchainPlatform::SwapchainPlatform(Allocator& persistentAllocator, Backend& backend, Device& device, Queue& queue, sys::SystemWindow& window, GraphicFormat format)
            : m_PersistentAllocator{ persistentAllocator }
            , m_OwnerDevice{ &device }
            , m_FlipQueue{ &queue }
            , m_Format{ format }
            , m_FramebufferIndex{}
            , m_FrameFence{}
            , m_CPUFrame{}
            , m_GPUFrame{}
        {
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
            swapChainDesc.Width = window.GetWidth();
            swapChainDesc.Height = window.GetHeight();
            swapChainDesc.Format = ConvertToResourceFormat(m_Format);
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = cfg::MaxFrameLatency();
            swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

            ComPtr<IDXGISwapChain1> swapChain1;
            hdEnsure(backend.GetNativeFactory()->CreateSwapChainForHwnd(
                queue.GetNativeQueue(),
                window.GetNativeHandle(),
                &swapChainDesc,
                nullptr,
                nullptr,
                &swapChain1));

            hdEnsure(swapChain1.As<IDXGISwapChain3>(&m_SwapChain));
            hdLogInfo(u8"Swap chain created %x% [%].", swapChainDesc.Width, swapChainDesc.Height, size_t(window.GetNativeHandle()));

            // Disable fullscreen transitions
            hdEnsure(backend.GetNativeFactory()->MakeWindowAssociation(window.GetNativeHandle(), DXGI_MWA_NO_ALT_ENTER));

            m_FramebufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

            CreateFramebufferTextures();

            m_FrameFence = hdNew(m_PersistentAllocator, Fence)(*m_OwnerDevice, 0U);
        }

        SwapchainPlatform::~SwapchainPlatform()
        {
            m_FlipQueue->Flush();

            ReleaseFrameBufferTextures();

            hdSafeDelete(m_PersistentAllocator, m_FrameFence);
        }

        void SwapchainPlatform::UpdateGPUFrame()
        {
            m_GPUFrame = m_FrameFence->GetValue();

            // Wait for GPU to not exceed maximum queued frames
            if (m_CPUFrame - m_GPUFrame >= cfg::MaxFrameLatency())
            {
                uint64_t gpuFrameToWait = m_CPUFrame - cfg::MaxFrameLatency() + 1;
                m_FrameFence->Wait(gpuFrameToWait);

                m_GPUFrame = m_FrameFence->GetValue();
            }
        }

        void SwapchainPlatform::CreateFramebufferTextures()
        {
            for (uint32_t framebufferIdx = 0U; framebufferIdx < cfg::MaxFrameLatency(); ++framebufferIdx)
            {
                ID3D12Resource* resource{};
                hdEnsure(m_SwapChain->GetBuffer(framebufferIdx, IID_PPV_ARGS(&resource)));
                m_FramebufferTextures[framebufferIdx] = m_OwnerDevice->RegisterTexture(resource, D3D12_RESOURCE_STATE_PRESENT, m_Format, TextureFlagsBits::RenderTarget, 
                    TextureDimenstion::Texture2D);
            }
        }

        void SwapchainPlatform::ReleaseFrameBufferTextures()
        {
            for (uint32_t framebufferIdx = 0; framebufferIdx < cfg::MaxFrameLatency(); ++framebufferIdx)
            {
                m_OwnerDevice->DestroyTextureImmediate(m_FramebufferTextures[framebufferIdx]);
            }
        }

        void Swapchain::Flip()
        {
            TextureHandle framebuffer;
            framebuffer = GetActiveFramebuffer();
            m_FlipQueue->PresentFrom(framebuffer);

            m_CPUFrame += 1;
            m_FlipQueue->Signal(*m_FrameFence, m_CPUFrame);

            m_SwapChain->Present(1, 0);
            m_FramebufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

            UpdateGPUFrame();
        }

        void Swapchain::Resize(uint32_t width, uint32_t height)
        {
            m_FlipQueue->Flush();
            UpdateGPUFrame();

            ReleaseFrameBufferTextures();

            DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
            hdEnsure(m_SwapChain->GetDesc1(&swapchainDesc));
            hdEnsure(m_SwapChain->ResizeBuffers(swapchainDesc.BufferCount, width, height, swapchainDesc.Format, swapchainDesc.Flags));

            CreateFramebufferTextures();

            m_FramebufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
        }

        TextureHandle Swapchain::GetActiveFramebuffer() const
        {
            return m_FramebufferTextures[m_FramebufferIndex];
        }

        uint64_t Swapchain::GetCPUFrame() const
        {
            return m_CPUFrame;
        }

        uint64_t Swapchain::GetGPUFrame() const
        {
            return m_GPUFrame;
        }
    }
}

#endif
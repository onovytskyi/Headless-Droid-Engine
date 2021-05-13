#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/Device.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Debug/Assert.h"
#include "Engine/Debug/Log.h"
#include "Engine/Framework/Graphics/Backend.h"
#include "Engine/Framework/Graphics/DX12/TextureDX12.h"

namespace hd
{
    namespace gfx
    {
        DevicePlatform::DevicePlatform(Backend& backend)
            : m_Backend{ &backend }
            , m_MessageCallbackCookie{}
        {
            m_Adapter = backend.GetBestAdapter();

            hdEnsure(::D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf())));

#if defined(HD_ENABLE_GFX_DEBUG)
            dbg::SetDebugDevice(m_Device.Get());
            ComPtr<ID3D12InfoQueue1> infoQueue1;
            if (SUCCEEDED(m_Device.As(&infoQueue1)))
            {
                infoQueue1->RegisterMessageCallback(dbg::D3D12DebugMessageCallback, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, nullptr, &m_MessageCallbackCookie);
            }
            else
            {
                ComPtr<ID3D12InfoQueue> infoQueue;
                if (SUCCEEDED(m_Device.As(&infoQueue)))
                {
                    hdEnsure(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE));
                    hdEnsure(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE));
                    hdEnsure(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE));

                    hdLogInfo(u8"DirectX break on warnings and errors enabled.");
                }
                else
                {
                    hdLogWarning(u8"Failed to enable breaks on DirectX warnings and errors.");
                }
            }
#endif
        }

        DevicePlatform::~DevicePlatform()
        {
#if defined(HD_ENABLE_GFX_DEBUG)
            dbg::SetDebugDevice(nullptr);

            ComPtr<ID3D12InfoQueue1> infoQueue1;
            if (SUCCEEDED(m_Device.As(&infoQueue1)))
            {
                infoQueue1->UnregisterMessageCallback(m_MessageCallbackCookie);
            }
#endif
        }

        ID3D12Device* DevicePlatform::GetNativeDevice() const
        {
            return m_Device.Get();
        }

        TextureHandle DevicePlatform::RegisterTexture(ID3D12Resource* resource, D3D12_RESOURCE_STATES state, GraphicFormat format, uint32_t flags)
        {
            Texture* texture{};
            TextureHandle result = TextureHandle(m_Backend->GetTextureAllocator().Allocate(&texture));

            new(texture)Texture(resource);

            return result;
        }

        TextureHandle Device::CreateTexture(uint64_t width, uint32_t height, uint16_t depth, uint16_t mipLevels, GraphicFormat format, uint32_t flags, TextureDimenstion dimension,
            float clearValue[4])
        {
            hdAssert(false, u8"Not implemented.");

            return TextureHandle{};
        }

        void Device::DestroyTexture(TextureHandle handle)
        {
            hdAssert(false, u8"Not implemented.");
        }

        void Device::DestroyTextureImmediate(TextureHandle handle)
        {
            util::VirtualPoolAllocator<Texture>& textureAllocator = m_Backend->GetTextureAllocator();

            Texture& texture = textureAllocator.Get(uint64_t(handle));
            texture.GetNativeResource()->Release();
            textureAllocator.Free(uint64_t(handle));
        }
    }
}

#endif
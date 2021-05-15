#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/TextureDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Debug/Assert.h"
#include "Engine/Framework/Graphics/Device.h"
#include "Engine/Framework/Graphics/DX12/UtilsDX12.h"

namespace hd
{
    namespace gfx
    {
        Texture::Texture(Device& device, ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState, GraphicFormat format, uint32_t flags)
            : m_Data{}
            , m_Format{ format }
        {
            m_Data.Resource = resource;
            m_Data.State = initialState;

            CreateViews(device, format, flags, resource->GetDesc().Dimension, false);
        }

        void Texture::FreeDescriptors(Device& device)
        {
            DescriptorManager& descriptorManager = device.GetDescriptorManager();

            if (m_RTV)
            {
                descriptorManager.Free(m_RTV);
            }

            if (m_DSV)
            {
                descriptorManager.Free(m_DSV);
            }

            if (m_SRV)
            {
                descriptorManager.Free(m_SRV);
            }

            if (m_UAV)
            {
                descriptorManager.Free(m_UAV);
            }

            for (size_t subresourceIdx = 0; subresourceIdx < ResourceStateTracker::StateTrackedData::MAX_SUBRESOURCES; ++subresourceIdx)
            {
                if (m_SubresourceSRV[subresourceIdx])
                {
                    descriptorManager.Free(m_SubresourceSRV[subresourceIdx]);
                }

                if (m_SubresourceUAV[subresourceIdx])
                {
                    descriptorManager.Free(m_SubresourceUAV[subresourceIdx]);
                }
            }
        }

        ID3D12Resource* Texture::GetNativeResource() const
        {
            return m_Data.Resource;
        }

        ResourceStateTracker::StateTrackedData& Texture::GetStateTrackedData()
        {
            return m_Data;
        }

        DescriptorRTV Texture::GetRTV() const
        {
            return m_RTV;
        }

        DescriptorDSV Texture::GetDSV() const
        {
            return m_DSV;
        }

        DescriptorSRV Texture::GetSRV() const
        {
            return m_SRV;
        }

        DescriptorSRV Texture::GetUAV() const
        {
            return m_UAV;
        }

        DescriptorSRV Texture::GetOrCreateSubresoruceSRV(Device& device, uint32_t subresourceIdx)
        {
            hdAssert(m_SRV, u8"Cannot create subresource SRV for non SRV capable texture.");

            if (subresourceIdx == ResourceStateTracker::ALL_SUBRESOURCES)
                return m_SRV;

            hdAssert(subresourceIdx < ResourceStateTracker::StateTrackedData::MAX_SUBRESOURCES, u8"Texture only support % subresources. Cannot get subresource with index %.", 
                ResourceStateTracker::StateTrackedData::MAX_SUBRESOURCES, subresourceIdx);

            if (!m_SubresourceSRV[subresourceIdx])
            {
                m_SubresourceSRV[subresourceIdx] = device.GetDescriptorManager().AllocateSRV();

                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
                srvDesc.ViewDimension = ResourceDimensionToSRV(m_Data.Resource->GetDesc().Dimension, false);
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                srvDesc.Format = ConvertToReadableFormat(m_Format);
                switch (srvDesc.ViewDimension)
                {
                case D3D12_SRV_DIMENSION_TEXTURE1D:
                    srvDesc.Texture1D.MipLevels = 1;
                    srvDesc.Texture1D.MostDetailedMip = subresourceIdx;
                    srvDesc.Texture1D.ResourceMinLODClamp = 0.0f;
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE2D:
                    srvDesc.Texture2D.MipLevels = 1;
                    srvDesc.Texture2D.MostDetailedMip = subresourceIdx;
                    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
                    srvDesc.Texture2D.PlaneSlice = 0;
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE3D:
                    srvDesc.Texture3D.MipLevels = 1;
                    srvDesc.Texture3D.MostDetailedMip = subresourceIdx;
                    srvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
                    break;
                default:
                    hdAssert(u8"Unknown texture view dimension.");
                    break;
                }

                device.GetNativeDevice()->CreateShaderResourceView(m_Data.Resource, &srvDesc, m_SubresourceSRV[subresourceIdx].HandleCPU);
            }

            return m_SubresourceSRV[subresourceIdx];
        }

        DescriptorSRV Texture::GetOrCreateSubresoruceUAV(Device& device, uint32_t subresourceIdx)
        {
            hdAssert(m_UAV, u8"Cannot create subresource UAV for non UAV capable texture.");

            if (subresourceIdx == ResourceStateTracker::ALL_SUBRESOURCES)
                return m_UAV;

            hdAssert(subresourceIdx < ResourceStateTracker::StateTrackedData::MAX_SUBRESOURCES, u8"Texture only support % subresources. Cannot get subresource with index %.", 
                ResourceStateTracker::StateTrackedData::MAX_SUBRESOURCES, subresourceIdx);

            if (m_SubresourceUAV[subresourceIdx])
            {
                m_SubresourceUAV[subresourceIdx] = device.GetDescriptorManager().AllocateSRV();

                D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
                uavDesc.ViewDimension = ResourceDimensionToUAV(m_Data.Resource->GetDesc().Dimension);
                uavDesc.Format = ConvertToWriteableFormat(m_Format);
                switch (uavDesc.ViewDimension)
                {
                case D3D12_UAV_DIMENSION_TEXTURE1D:
                    uavDesc.Texture1D.MipSlice = subresourceIdx;
                    break;
                case D3D12_UAV_DIMENSION_TEXTURE2D:
                    uavDesc.Texture2D.MipSlice = subresourceIdx;
                    uavDesc.Texture2D.PlaneSlice = 0;
                    break;
                case D3D12_UAV_DIMENSION_TEXTURE3D:
                    uavDesc.Texture3D.MipSlice = subresourceIdx;
                    uavDesc.Texture3D.FirstWSlice = 0;
                    uavDesc.Texture3D.WSize = 1;
                    break;
                default:
                    hdAssert(u8"Unknown texture view dimension.");
                    break;
                }

                device.GetNativeDevice()->CreateUnorderedAccessView(m_Data.Resource, nullptr, &uavDesc, m_SubresourceUAV[subresourceIdx].HandleCPU);
            }

            return m_SubresourceUAV[subresourceIdx];
        }

        void Texture::CreateViews(Device& device, GraphicFormat format, uint32_t flags, D3D12_RESOURCE_DIMENSION dimension, bool isCube)
        {
            DescriptorManager& descriptorManager = device.GetDescriptorManager();

            if (flags & uint32_t(TextureFlags::RenderTarget))
            {
                m_RTV = descriptorManager.AllocateRTV();

                D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
                rtvDesc.ViewDimension = ResourceDimensionToRTV(dimension);
                rtvDesc.Format = ConvertToWriteableFormat(m_Format);
                switch (rtvDesc.ViewDimension)
                {
                case D3D12_RTV_DIMENSION_TEXTURE1D:
                    rtvDesc.Texture1D.MipSlice = 0;
                    break;
                case D3D12_RTV_DIMENSION_TEXTURE2D:
                    rtvDesc.Texture2D.MipSlice = 0;
                    rtvDesc.Texture2D.PlaneSlice = 0;
                    break;
                case D3D12_RTV_DIMENSION_TEXTURE3D:
                    rtvDesc.Texture3D.MipSlice = 0;
                    rtvDesc.Texture3D.FirstWSlice = 0;
                    rtvDesc.Texture3D.WSize = 1;
                    break;
                default:
                    hdAssert(u8"Unknown texture view dimension.");
                    break;
                }
                device.GetNativeDevice()->CreateRenderTargetView(m_Data.Resource, &rtvDesc, m_RTV.HandleCPU);
            }

            if (flags & uint32_t(TextureFlags::DepthStencil))
            {
                m_DSV = descriptorManager.AllocateDSV();

                D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
                dsvDesc.ViewDimension = ResourceDimensionToDSV(dimension);
                dsvDesc.Format = ConvertToWriteableFormat(m_Format);
                dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
                switch (dsvDesc.ViewDimension)
                {
                case D3D12_DSV_DIMENSION_TEXTURE1D:
                    dsvDesc.Texture1D.MipSlice = 0;
                    break;
                case D3D12_DSV_DIMENSION_TEXTURE2D:
                    dsvDesc.Texture2D.MipSlice = 0;
                    break;
                default:
                    hdAssert(u8"Unknown texture view dimension.");
                    break;
                }
                device.GetNativeDevice()->CreateDepthStencilView(m_Data.Resource, &dsvDesc, m_DSV.HandleCPU);
            }

            if (flags & uint32_t(TextureFlags::ShaderResource))
            {
                m_SRV = descriptorManager.AllocateSRV();

                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
                srvDesc.ViewDimension = ResourceDimensionToSRV(dimension, isCube);
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                srvDesc.Format = ConvertToReadableFormat(m_Format);
                switch (srvDesc.ViewDimension)
                {
                case D3D12_SRV_DIMENSION_TEXTURE1D:
                    srvDesc.Texture1D.MipLevels = -1;
                    srvDesc.Texture1D.MostDetailedMip = 0;
                    srvDesc.Texture1D.ResourceMinLODClamp = 0.0f;
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE2D:
                    srvDesc.Texture2D.MipLevels = -1;
                    srvDesc.Texture2D.MostDetailedMip = 0;
                    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
                    srvDesc.Texture2D.PlaneSlice = 0;
                    break;
                case D3D12_SRV_DIMENSION_TEXTURECUBE:
                    srvDesc.TextureCube.MipLevels = -1;
                    srvDesc.TextureCube.MostDetailedMip = 0;
                    srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
                    break;
                case D3D12_SRV_DIMENSION_TEXTURE3D:
                    srvDesc.Texture3D.MipLevels = -1;
                    srvDesc.Texture3D.MostDetailedMip = 0;
                    srvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
                    break;
                default:
                    hdAssert(u8"Unknown texture view dimension.");
                    break;
                }
                device.GetNativeDevice()->CreateShaderResourceView(m_Data.Resource, &srvDesc, m_SRV.HandleCPU);
            }

            if (flags & uint32_t(TextureFlags::UnorderedAccess))
            {
                hdAssert(!isCube, u8"Writing to cubemaps is not supported.");

                m_UAV = descriptorManager.AllocateSRV();

                D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
                uavDesc.ViewDimension = ResourceDimensionToUAV(dimension);
                uavDesc.Format = ConvertToWriteableFormat(m_Format);
                switch (uavDesc.ViewDimension)
                {
                case D3D12_UAV_DIMENSION_TEXTURE1D:
                    uavDesc.Texture1D.MipSlice = 0;
                    break;
                case D3D12_UAV_DIMENSION_TEXTURE2D:
                    uavDesc.Texture2D.MipSlice = 0;
                    uavDesc.Texture2D.PlaneSlice = 0;
                    break;
                case D3D12_UAV_DIMENSION_TEXTURE3D:
                    uavDesc.Texture3D.MipSlice = 0;
                    uavDesc.Texture3D.FirstWSlice = 0;
                    uavDesc.Texture3D.WSize = 1;
                    break;
                default:
                    hdAssert(u8"Unknown texture view dimension.");
                    break;
                }
                device.GetNativeDevice()->CreateUnorderedAccessView(m_Data.Resource, nullptr, &uavDesc, m_UAV.HandleCPU);
            }
        }
    }
}

#endif
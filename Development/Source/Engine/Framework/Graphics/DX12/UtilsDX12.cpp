#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/UtilsDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Debug/Assert.h"

namespace hd
{
    namespace gfx
    {
        DXGI_FORMAT ConvertToResourceFormat(GraphicFormat format)
        {
            switch (format)
            {
            case GraphicFormat::RGBA8UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case GraphicFormat::RGBA8UNorm_Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM;
            }

            return DXGI_FORMAT_UNKNOWN;
        }

        DXGI_FORMAT ConvertToWriteableFormat(GraphicFormat format)
        {
            switch (format)
            {
            case GraphicFormat::RGBA8UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case GraphicFormat::RGBA8UNorm_Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            }

            return DXGI_FORMAT_UNKNOWN;
        }

        DXGI_FORMAT ConvertToReadableFormat(GraphicFormat format)
        {
            switch (format)
            {
            case GraphicFormat::RGBA8UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case GraphicFormat::RGBA8UNorm_Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            }

            return DXGI_FORMAT_UNKNOWN;
        }

        bool IsBlockCompressed(GraphicFormat format)
        {
            return false;
        }

        size_t GetBlockSize(GraphicFormat format)
        {
            return 16;
        }

        size_t BytesPerElement(GraphicFormat format)
        {
            switch (format)
            {
            case GraphicFormat::RGBA8UNorm: return 4;
            case GraphicFormat::RGBA8UNorm_Srgb: return 4;
            }

            hdAssert(false, u8"Cannot get BPP for format. Format not supported!");
            return 0;
        }

        D3D12_SRV_DIMENSION ResourceDimensionToSRV(D3D12_RESOURCE_DIMENSION dimension, bool isCube)
        {
            switch (dimension)
            {
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: return D3D12_SRV_DIMENSION_TEXTURE1D;
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: return isCube ? D3D12_SRV_DIMENSION_TEXTURECUBE : D3D12_SRV_DIMENSION_TEXTURE2D;
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D: return D3D12_SRV_DIMENSION_TEXTURE3D;
            default:
                hdAssert(u8"Unknown texture resoure dimension");
                break;
            }

            return D3D12_SRV_DIMENSION_UNKNOWN;
        }

        D3D12_UAV_DIMENSION ResourceDimensionToUAV(D3D12_RESOURCE_DIMENSION dimension)
        {
            switch (dimension)
            {
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: return D3D12_UAV_DIMENSION_TEXTURE1D;
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: return D3D12_UAV_DIMENSION_TEXTURE2D;
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D: return D3D12_UAV_DIMENSION_TEXTURE3D;
            default:
                hdAssert(u8"Unknown texture resoure dimension");
                break;
            }

            return D3D12_UAV_DIMENSION_UNKNOWN;
        }

        D3D12_RTV_DIMENSION ResourceDimensionToRTV(D3D12_RESOURCE_DIMENSION dimension)
        {
            switch (dimension)
            {
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: return D3D12_RTV_DIMENSION_TEXTURE1D;
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: return D3D12_RTV_DIMENSION_TEXTURE2D;
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D: return D3D12_RTV_DIMENSION_TEXTURE3D;
            default:
                hdAssert(u8"Unknown texture resoure dimension");
                break;
            }

            return D3D12_RTV_DIMENSION_UNKNOWN;
        }

        D3D12_DSV_DIMENSION ResourceDimensionToDSV(D3D12_RESOURCE_DIMENSION dimension)
        {
            switch (dimension)
            {
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: return D3D12_DSV_DIMENSION_TEXTURE1D;
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: return D3D12_DSV_DIMENSION_TEXTURE2D;
            default:
                hdAssert(u8"Unknown texture resoure dimension");
                break;
            }

            return D3D12_DSV_DIMENSION_UNKNOWN;
        }

        D3D12_RESOURCE_DIMENSION ConvertToResourceDimension(TextureDimenstion dimension)
        {
            switch (dimension)
            {
            case hd::gfx::TextureDimenstion::Texture1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
            case hd::gfx::TextureDimenstion::Texture2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            case hd::gfx::TextureDimenstion::Texture3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
            case hd::gfx::TextureDimenstion::TextureCube: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            default:
                hdAssert(u8"Unknown texture resoure dimension");
                break;
            }

            return D3D12_RESOURCE_DIMENSION_UNKNOWN;
        }
    }
}

#endif
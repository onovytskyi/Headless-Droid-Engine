#include "Config/Bootstrap.h"

#include "Framework/Graphics/DX12/UtilsDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Debug/Assert.h"

namespace hd
{
    namespace gfx
    {
        DXGI_FORMAT ConvertToResourceFormat(GraphicFormat format)
        {
            switch (format)
            {
            case GraphicFormat::D24UNorm_S8UInt: return DXGI_FORMAT_D24_UNORM_S8_UINT;
            case GraphicFormat::RGBA8UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case GraphicFormat::RGBA8UNorm_Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case GraphicFormat::BGRA8Unorm: return DXGI_FORMAT_B8G8R8A8_UNORM;
            case GraphicFormat::BGRA8Unorm_Srgb: return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
            case GraphicFormat::BGRX8Unorm: return DXGI_FORMAT_B8G8R8X8_UNORM;
            case GraphicFormat::BGRX8Unorm_Srgb: return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
            case GraphicFormat::BC1Unorm: return DXGI_FORMAT_BC1_UNORM;
            case GraphicFormat::BC2Unorm: return DXGI_FORMAT_BC2_UNORM;
            case GraphicFormat::BC3Unorm: return DXGI_FORMAT_BC3_UNORM;
            case GraphicFormat::BC4Unorm: return DXGI_FORMAT_BC4_UNORM;
            case GraphicFormat::BC4Snorm: return DXGI_FORMAT_BC4_SNORM;
            case GraphicFormat::BC5Unorm: return DXGI_FORMAT_BC5_UNORM;
            case GraphicFormat::BC5Snorm: return DXGI_FORMAT_BC5_SNORM;
            case GraphicFormat::RGBA16Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
            case GraphicFormat::RGBA32Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
            }

            return DXGI_FORMAT_UNKNOWN;
        }

        DXGI_FORMAT ConvertToWriteableFormat(GraphicFormat format)
        {
            switch (format)
            {
            case GraphicFormat::D24UNorm_S8UInt: return DXGI_FORMAT_D24_UNORM_S8_UINT;
            case GraphicFormat::RGBA8UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case GraphicFormat::RGBA8UNorm_Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            case GraphicFormat::RGBA16Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
            case GraphicFormat::RGBA32Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
            }

            return DXGI_FORMAT_UNKNOWN;
        }

        DXGI_FORMAT ConvertToReadableFormat(GraphicFormat format)
        {
            switch (format)
            {
            case GraphicFormat::D24UNorm_S8UInt: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            case GraphicFormat::RGBA8UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case GraphicFormat::BGRA8Unorm: return DXGI_FORMAT_B8G8R8A8_UNORM;
            case GraphicFormat::BGRX8Unorm: return DXGI_FORMAT_B8G8R8X8_UNORM;
            case GraphicFormat::BC1Unorm: return DXGI_FORMAT_BC1_UNORM;
            case GraphicFormat::BC2Unorm: return DXGI_FORMAT_BC2_UNORM;
            case GraphicFormat::BC3Unorm: return DXGI_FORMAT_BC3_UNORM;
            case GraphicFormat::BC4Unorm: return DXGI_FORMAT_BC4_UNORM;
            case GraphicFormat::BC4Snorm: return DXGI_FORMAT_BC4_SNORM;
            case GraphicFormat::BC5Unorm: return DXGI_FORMAT_BC5_UNORM;
            case GraphicFormat::BC5Snorm: return DXGI_FORMAT_BC5_SNORM;
            case GraphicFormat::RGBA16Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
            case GraphicFormat::RGBA32Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
            }

            return DXGI_FORMAT_UNKNOWN;
        }

        bool IsBlockCompressed(GraphicFormat format)
        {
            switch (format)
            {
            case GraphicFormat::BC1Unorm:
            case GraphicFormat::BC2Unorm:
            case GraphicFormat::BC3Unorm:
            case GraphicFormat::BC4Unorm:
            case GraphicFormat::BC4Snorm:
            case GraphicFormat::BC5Unorm:
            case GraphicFormat::BC5Snorm:
                return true;
            }

            return false;
        }

        size_t GetBlockSize(GraphicFormat format)
        {
            switch (format)
            {
            case GraphicFormat::BC1Unorm:
            case GraphicFormat::BC4Unorm:
                return 8;
            }

            return 16;
        }

        size_t BytesPerElement(GraphicFormat format)
        {
            switch (format)
            {
            case GraphicFormat::D24UNorm_S8UInt: return 4;
            case GraphicFormat::RGBA8UNorm: return 4;
            case GraphicFormat::RGBA8UNorm_Srgb: return 4;
            case GraphicFormat::RGBA16Float: return 8;
            case GraphicFormat::RGBA32Float: return 16;
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

        D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertToTopologyType(PrimitiveType primitiveType)
        {
            switch (primitiveType)
            {
            case hd::gfx::PrimitiveType::Point: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
            case hd::gfx::PrimitiveType::Line: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
            case hd::gfx::PrimitiveType::Triangle: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            case hd::gfx::PrimitiveType::Patch: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
            default:
                hdAssert(u8"Unknown primitive type");
                break;
            }

            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
        }

        D3D_PRIMITIVE_TOPOLOGY ConvertToPrimitiveTopology(PrimitiveType primitiveType, TopologyType topologyType)
        {
            switch (primitiveType)
            {
            case hd::gfx::PrimitiveType::Point:
            {
                switch (topologyType)
                {
                case hd::gfx::TopologyType::List: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
                }
            }
            break;

            case hd::gfx::PrimitiveType::Line:
            {
                switch (topologyType)
                {
                case hd::gfx::TopologyType::List: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
                case hd::gfx::TopologyType::Strip: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
                case hd::gfx::TopologyType::ListAdjacent: return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
                case hd::gfx::TopologyType::StipAdjacent: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
                }
            }
            break;

            case hd::gfx::PrimitiveType::Triangle:
            {
                switch (topologyType)
                {
                case hd::gfx::TopologyType::List: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
                case hd::gfx::TopologyType::Strip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
                case hd::gfx::TopologyType::ListAdjacent: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
                case hd::gfx::TopologyType::StipAdjacent: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
                }
            }
            break;

            case hd::gfx::PrimitiveType::Patch:
            {
                switch (topologyType)
                {
                case hd::gfx::TopologyType::List: return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
                }
            }
            default:
                hdAssert(u8"Unknown primitive type");
                break;
            }

            return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        }

        void FillBlendParameters(BlendType type, D3D12_BLEND& srcBlend, D3D12_BLEND& destBlend, D3D12_BLEND_OP& blendOp)
        {
            switch (type)
            {
            case hd::gfx::BlendType::None:
            {
                srcBlend = D3D12_BLEND_ONE;
                destBlend = D3D12_BLEND_ZERO;
                blendOp = D3D12_BLEND_OP_ADD;
            }
            break;

            case hd::gfx::BlendType::Alpha:
            {
                srcBlend = D3D12_BLEND_SRC_ALPHA;
                destBlend = D3D12_BLEND_INV_SRC_ALPHA;
                blendOp = D3D12_BLEND_OP_ADD;
            }
            break;

            case hd::gfx::BlendType::AlphaSource:
            {
                srcBlend = D3D12_BLEND_INV_SRC_ALPHA;
                destBlend = D3D12_BLEND_ZERO;
                blendOp = D3D12_BLEND_OP_ADD;
            }
            break;

            default:
                hdAssert(u8"Unknown blend type");
                break;
            }
        }

        D3D12_RENDER_TARGET_BLEND_DESC ConstructBlendDesc(BlendType color, BlendType alpha)
        {
            D3D12_RENDER_TARGET_BLEND_DESC result{};

            result.BlendEnable = color != BlendType::None || alpha != BlendType::None;
            result.LogicOpEnable = FALSE;

            FillBlendParameters(color, result.SrcBlend, result.DestBlend, result.BlendOp);
            FillBlendParameters(alpha, result.SrcBlendAlpha, result.DestBlendAlpha, result.BlendOpAlpha);

            result.LogicOp = D3D12_LOGIC_OP_NOOP;
            result.RenderTargetWriteMask = (color != BlendType::None ? ((D3D12_COLOR_WRITE_ENABLE_RED | D3D12_COLOR_WRITE_ENABLE_GREEN) | D3D12_COLOR_WRITE_ENABLE_BLUE ) : 0);
            result.RenderTargetWriteMask |= (alpha != BlendType::None ? D3D12_COLOR_WRITE_ENABLE_ALPHA : 0);

            return result;
        }

        D3D12_COMPARISON_FUNC ConvertToComparisonFunc(ComparisonFunc comparisonFunc)
        {
            switch (comparisonFunc)
            {
            case hd::gfx::ComparisonFunc::Never: return D3D12_COMPARISON_FUNC_NEVER;
            case hd::gfx::ComparisonFunc::Less: return D3D12_COMPARISON_FUNC_LESS;
            case hd::gfx::ComparisonFunc::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
            case hd::gfx::ComparisonFunc::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
            case hd::gfx::ComparisonFunc::Greater: return D3D12_COMPARISON_FUNC_GREATER;
            case hd::gfx::ComparisonFunc::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
            case hd::gfx::ComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
            case hd::gfx::ComparisonFunc::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
            default:
                hdAssert(u8"Unknown comparison func type");
                break;
            }

            return D3D12_COMPARISON_FUNC_ALWAYS;
        }

    }
}

#endif
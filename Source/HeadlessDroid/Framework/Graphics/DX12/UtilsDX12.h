#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace gfx
    {
        DXGI_FORMAT ConvertToResourceFormat(GraphicFormat format);
        DXGI_FORMAT ConvertToWriteableFormat(GraphicFormat format);
        DXGI_FORMAT ConvertToReadableFormat(GraphicFormat format);

        bool IsBlockCompressed(GraphicFormat format);
        size_t GetBlockSize(GraphicFormat format);
        size_t BytesPerElement(GraphicFormat format);

        D3D12_SRV_DIMENSION ResourceDimensionToSRV(D3D12_RESOURCE_DIMENSION dimension, bool isCube);
        D3D12_UAV_DIMENSION ResourceDimensionToUAV(D3D12_RESOURCE_DIMENSION dimension);
        D3D12_RTV_DIMENSION ResourceDimensionToRTV(D3D12_RESOURCE_DIMENSION dimension);
        D3D12_DSV_DIMENSION ResourceDimensionToDSV(D3D12_RESOURCE_DIMENSION dimension);

        D3D12_RESOURCE_DIMENSION ConvertToResourceDimension(TextureDimenstion dimension);
        D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertToTopologyType(PrimitiveType primitiveType);
        D3D_PRIMITIVE_TOPOLOGY ConvertToPrimitiveTopology(PrimitiveType primitiveType, TopologyType topologyType);

        D3D12_RENDER_TARGET_BLEND_DESC ConstructBlendDesc(BlendType color, BlendType alpha);
        D3D12_COMPARISON_FUNC ConvertToComparisonFunc(ComparisonFunc comparisonFunc);
    }
}

#endif
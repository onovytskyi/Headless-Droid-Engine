#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace gfx
    {
        namespace dx12
        {
            DXGI_FORMAT ConvertToResourceFormat(GraphicFormat format);
            DXGI_FORMAT ConvertToWriteableFormat(GraphicFormat format);
            DXGI_FORMAT ConvertToReadableFormat(GraphicFormat format);
        }
    }
}

#endif
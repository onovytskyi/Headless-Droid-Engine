#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/UtilsDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

namespace hd
{
    namespace gfx
    {
        namespace dx12
        {

            DXGI_FORMAT ConvertToResourceFormat(GraphicFormat format)
            {
                switch (format)
                {
                case GraphicFormat::RGBA8UNorm_Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM;
                }

                return DXGI_FORMAT_UNKNOWN;
            }

            DXGI_FORMAT ConvertToWriteableFormat(GraphicFormat format)
            {
                switch (format)
                {
                case GraphicFormat::RGBA8UNorm_Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                }

                return DXGI_FORMAT_UNKNOWN;
            }

            DXGI_FORMAT ConvertToReadableFormat(GraphicFormat format)
            {
                switch (format)
                {
                case GraphicFormat::RGBA8UNorm_Srgb: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
                }

                return DXGI_FORMAT_UNKNOWN;
            }
        }
    }
}

#endif
#pragma once

#if defined(HD_GRAPHICS_API_DX12)

namespace hd
{
    namespace gfx
    {
        class Texture
        {
        public:
            Texture(ID3D12Resource* resource);

            ID3D12Resource* GetNativeResource() const;

        private:
            ID3D12Resource* m_Resource;
        };
    }
}

#endif
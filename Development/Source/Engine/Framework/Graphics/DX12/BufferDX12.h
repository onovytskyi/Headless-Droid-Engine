#pragma once

#if defined(HD_GRAPHICS_API_DX12)

namespace hd
{
    namespace gfx
    {
        class Buffer
        {
        public:
            Buffer(ID3D12Resource* resource);

            ID3D12Resource* GetNativeResource() const;

        private:
            ID3D12Resource* m_Resource;
        };
    }
}

#endif
#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/DX12/ResourceStateTrackerDX12.h"

namespace hd
{
    namespace gfx
    {
        class Texture
        {
        public:
            Texture(ID3D12Resource* resource);
            Texture(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState);

            hdNoncopyable(Texture)

            ID3D12Resource* GetNativeResource() const;
            ResourceStateTracker::StateTrackedData& GetStateTrackedData();

        private:
            ResourceStateTracker::StateTrackedData m_Data;
        };
    }
}

#endif
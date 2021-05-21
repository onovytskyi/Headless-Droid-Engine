#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/DX12/ResourceStateTrackerDX12.h"

namespace hd
{
    namespace gfx
    {
        class Device;

        class Buffer
        {
        public:
            Buffer(ID3D12Resource* resource);
            Buffer(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState);

            hdNoncopyable(Buffer)

            void Free(Device& device);

            ID3D12Resource* GetNativeResource() const;
            ResourceStateTracker::StateTrackedData& GetStateTrackedData();

        private:
            ResourceStateTracker::StateTrackedData m_Data;
        };
    }
}

#endif
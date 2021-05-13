#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace gfx
    {
        class Device;

        class QueuePlatform
        {
        public:
            QueuePlatform(Device& device, QueueType type);
            ~QueuePlatform();

            ID3D12CommandQueue* GetNativeQueue() const;

        protected:
            ComPtr<ID3D12CommandQueue> m_Queue;
        };
    }
}

#endif
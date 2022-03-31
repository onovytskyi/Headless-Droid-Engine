#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Graphics/GraphicsTypes.h"

namespace hd
{
	class Device;

	class QueuePlatform
	{
	public:
		QueuePlatform(Device& device, QueueType type);
		~QueuePlatform();

		ID3D12CommandQueue* GetNativeQueue() const;

		void PresentFrom(TextureHandle framebuffer);

	protected:
		Device* m_OwnerDevice;
		ComPtr<ID3D12CommandQueue> m_Queue;
	};
}

#endif

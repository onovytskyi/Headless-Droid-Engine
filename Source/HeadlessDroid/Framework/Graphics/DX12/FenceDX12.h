#pragma once

#if defined(HD_GRAPHICS_API_DX12)

namespace hd
{
	class Device;

	class FencePlatform
	{
	public:
		FencePlatform(Device& device, uint64_t initialValue);
		~FencePlatform();

		ID3D12Fence* GetNativeFence() const;

	protected:
		ComPtr<ID3D12Fence> m_Fence;
		HANDLE m_WaitEvent;
	};
}

#endif

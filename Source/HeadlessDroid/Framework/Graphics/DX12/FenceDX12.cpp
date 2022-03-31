#include "Config/Bootstrap.h"

#include "Framework/Graphics/Fence.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Debug/Assert.h"
#include "Framework/Graphics/Device.h"

namespace hd
{
	FencePlatform::FencePlatform(Device& device, uint64_t initialValue)
	{
		hdEnsure(device.GetNativeDevice()->CreateFence(initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.GetAddressOf())));

		m_WaitEvent = ::CreateEventW(nullptr, false, false, nullptr);
		hdEnsure(m_WaitEvent != INVALID_HANDLE_VALUE);
	}

	FencePlatform::~FencePlatform()
	{
		::CloseHandle(m_WaitEvent);
	}

	ID3D12Fence* FencePlatform::GetNativeFence() const
	{
		return m_Fence.Get();
	}

	uint64_t Fence::GetValue() const
	{
		return m_Fence->GetCompletedValue();
	}

	void Fence::Wait(uint64_t targetValue)
	{
		hdEnsure(m_Fence->SetEventOnCompletion(targetValue, m_WaitEvent));
		WaitForSingleObject(m_WaitEvent, INFINITE);
	}
}

#endif

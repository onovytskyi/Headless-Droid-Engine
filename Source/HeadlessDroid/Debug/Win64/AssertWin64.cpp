#include "Config/Bootstrap.h"

#if defined(HD_PLATFORM_WIN64)

#include "Debug/Assert.h"
#include "Foundation/String/StringConverter.h"

namespace hd
{
	void AssertHandlerInternal(HRESULT errorCode, char8_t const* assertion, char8_t const* file, uint32_t line)
	{
		size_t marker = g_AssertAllocator.GetMarker();

#if defined(HD_GRAPHICS_API_DX12) && defined(HD_ENABLE_GFX_DEBUG)
		errorCode = ConvertToGfxDeviceResult(errorCode);
#endif

		_com_error error(errorCode);
		wchar_t const* messageWide = error.ErrorMessage();

		size_t messageBufferSize = SizeAsNarrow(messageWide);
		char8_t* message = reinterpret_cast<char8_t*>(g_AssertAllocator.Allocate(messageBufferSize, 1));

		ToNarrow(messageWide, message, messageBufferSize);

		AssertHandlerFinal(assertion, file, line, message);

		g_AssertAllocator.Reset(marker);
	}

#if defined(HD_GRAPHICS_API_DX12) && defined(HD_ENABLE_GFX_DEBUG)
	static ID3D12Device* g_DebugDevice = nullptr;
	void SetDebugDevice(ID3D12Device* device)
	{
		g_DebugDevice = device;
	}

	HRESULT ConvertToGfxDeviceResult(HRESULT errorCode)
	{
		if (errorCode == DXGI_ERROR_DEVICE_REMOVED)
		{
			if (g_DebugDevice)
			{
				errorCode = g_DebugDevice->GetDeviceRemovedReason();
			}
		}

		return errorCode;
	}

	void D3D12DebugMessageCallback(D3D12_MESSAGE_CATEGORY category, D3D12_MESSAGE_SEVERITY severiry, D3D12_MESSAGE_ID id, LPCSTR description, void* context)
	{

	}

#endif

	void BreakIntoDebugger()
	{
		if (::IsDebuggerPresent())
		{
			::DebugBreak();
		}
	}
}

#endif

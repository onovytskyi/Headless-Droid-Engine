#include "Config/Bootstrap.h"

#include "Framework/Graphics/Backend.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Debug/Assert.h"
#include "Debug/Log.h"
#include "Foundation/Memory/Utils.h"
#include "Framework/Graphics/DX12/ShaderManagerDX12.h"

namespace hd
{
	BufferHandle INVALID_BUFFER_HANDLE = BufferHandle(VirtualPoolAllocator<GfxBuffer>::InvalidHandle());
	TextureHandle INVALID_TEXTURE_HANDLE = TextureHandle(VirtualPoolAllocator<GfxBuffer>::InvalidHandle());

	BackendPlatform::BackendPlatform(Allocator& persistentAllocator)
		: m_PersistentAllocator{ persistentAllocator }
		  , m_ShaderManager{}
		  , m_BufferAllocator{ 4096 }
		  , m_TextureAllocator{ 4096 }
	{
#if defined(HD_ENABLE_GFX_DEBUG)
		{
			ComPtr<ID3D12Debug> debugInterface;
			if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
			{
				debugInterface->EnableDebugLayer();
				hdLogInfo(u8"DirectX debug layer enabled.");

#if defined(HD_ENABLE_GFX_GPU_VALIDATION)
				ComPtr<ID3D12Debug3> debugInterface3;
				if (SUCCEEDED(debugInterface.As(&debugInterface3)))
				{
					debugInterface3->SetEnableGPUBasedValidation(TRUE);
					debugInterface3->SetEnableSynchronizedCommandQueueValidation(TRUE);
					hdLogInfo(u8"DirectX GPU based validation enabled.");
				}
				else
				{
					hdLogWarning(u8"Failed to enable DirectX GPU based validation.");
				}
#endif
			}
			else
			{
				hdLogWarning(u8"Failed to enable DirectX debug layer. Make sure Graphics Tools feature installed.");
			}
		}
#endif

		UINT dxgiFactoryFlags = 0;
#if defined(HD_ENABLE_GFX_DEBUG)
		// Enable additional debug layers.
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		ComPtr<IDXGIFactory2> factory2;
		hdEnsure(::CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory2.GetAddressOf())));
		hdEnsure(factory2.As<IDXGIFactory6>(&m_Factory));

		m_ShaderManager = hdNew(m_PersistentAllocator, ShaderManager)();
	}

	BackendPlatform::~BackendPlatform()
	{
		hdSafeDelete(m_PersistentAllocator, m_ShaderManager);

		m_Factory.Reset();

#if defined(HD_ENABLE_GFX_DEBUG)
		{
			ComPtr<IDXGIDebug1> dxgiDebug1;
			if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug1))))
			{
				dxgiDebug1->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
			}
		}
#endif
	}

	ComPtr<IDXGIAdapter4> BackendPlatform::GetBestAdapter() const
	{
		ComPtr<IDXGIAdapter> adapter;
		hdEnsure(m_Factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf())), u8"Unsupported Windows version.");

		ComPtr<IDXGIAdapter4> adapter4;
		hdEnsure(adapter.As<IDXGIAdapter4>(&adapter4), u8"Unsupported Windows version.");

#if defined(HD_ENABLE_LOG)
		DXGI_ADAPTER_DESC3 adapterDesc{};
		if (SUCCEEDED(adapter4->GetDesc3(&adapterDesc)))
		{
			hdLogInfo(u8"----------------------------------");
			hdLogInfo(u8"Adapter detected", );
			hdLogInfo(u8"Name: %", adapterDesc.Description);
			hdLogInfo(u8"ID: %.%.%.%", adapterDesc.VendorId, adapterDesc.DeviceId, adapterDesc.SubSysId, adapterDesc.Revision);
			hdLogInfo(u8"Videomem: % Mb", ToMB(size_t(adapterDesc.DedicatedVideoMemory)));
			hdLogInfo(u8"Systemmem: % Mb", ToMB(size_t(adapterDesc.DedicatedSystemMemory)));
			hdLogInfo(u8"Sharedmem: % Mb", ToMB(size_t(adapterDesc.SharedSystemMemory)));
			hdLogInfo(u8"----------------------------------");
		}
		else
		{
			hdLogWarning(u8"Failed to retrieve adapter description.");
		}
#endif

		return adapter4;
	}

	IDXGIFactory6* BackendPlatform::GetNativeFactory() const
	{
		return m_Factory.Get();
	}

	ShaderManager& BackendPlatform::GetShaderManager()
	{
		return *m_ShaderManager;
	}

	VirtualPoolAllocator<GfxBuffer>& BackendPlatform::GetBufferAllocator()
	{
		return m_BufferAllocator;
	}

	VirtualPoolAllocator<Texture>& BackendPlatform::GetTextureAllocator()
	{
		return m_TextureAllocator;
	}
}

#endif

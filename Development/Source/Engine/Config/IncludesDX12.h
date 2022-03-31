#pragma once

#if !defined(HD_ENABLE_GFX_DEBUG) && !defined(HD_ENABLE_GFX_GPU_VALIDATION)
#   define D3D12_IGNORE_SDK_LAYERS
#endif

#include "d3d12.h"

#if defined(HD_ENABLE_GFX_DEBUG)
#   include <dxgidebug.h>
#   pragma comment(lib, "dxguid.lib")
#endif

#include <dxgi1_6.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#if defined(HD_ENABLE_RESOURCE_COOKING)
#   include "d3d12shader.h"
#   include "Externals/DXC/dxcapi.h"
#   pragma comment(lib, "dxcompiler.lib")
#endif

#if defined(HD_ENGINE_PROJECT)
extern "C" { __declspec(dllexport) extern inline constexpr UINT D3D12SDKVersion = 4; }
extern "C" { __declspec(dllexport) extern inline const char* D3D12SDKPath = ".\\"; }
#endif
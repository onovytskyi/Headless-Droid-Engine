#pragma once

#if defined(HD_PLATFORM_WIN64)

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>
#include <comdef.h>

#include <DirectXMath.h>

#undef CreateWindow
#undef WriteConsole
#undef LoadImage
#undef min
#undef max
#undef near
#undef far

#endif
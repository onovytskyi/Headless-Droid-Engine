#pragma once

#if defined(HD_PLATFORM_WIN64)

namespace hd
{
    namespace math
    {
        using Matrix4x4 = DirectX::XMFLOAT4X4;
    }
}

#endif
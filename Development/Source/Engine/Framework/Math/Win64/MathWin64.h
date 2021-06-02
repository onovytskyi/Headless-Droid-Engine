#pragma once

#if defined(HD_PLATFORM_WIN64)

namespace hd
{
    namespace math
    {
        using Vectorf2 = DirectX::XMFLOAT2;
        using Vectorf3 = DirectX::XMFLOAT3;
        using Vectorf4 = DirectX::XMFLOAT4;
        using Matrix4x4 = DirectX::XMFLOAT4X4;
    }
}

#endif
#pragma once

#if defined(HD_PLATFORM_WIN64)

namespace hd
{
    namespace mem
    {
        class VirtualBufferPlatform
        {
        protected:
            VirtualBufferPlatform();

            static size_t s_PageSize;
            static size_t s_AllocationGranularity;
        };
    }
}

#endif
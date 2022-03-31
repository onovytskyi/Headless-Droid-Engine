#include "Config/Bootstrap.h"

#if defined(HD_PLATFORM_WIN64)

#include "Framework/Memory/VirtualBuffer.h"

#include "Debug/Assert.h"
#include "Foundation/Memory/Utils.h"

namespace hd
{
    namespace mem
    {
        size_t VirtualBufferPlatform::s_PageSize = 0;
        size_t VirtualBufferPlatform::s_AllocationGranularity = 0;

        VirtualBufferPlatform::VirtualBufferPlatform()
        {
            if (s_PageSize == 0 || s_AllocationGranularity == 0)
            {
                SYSTEM_INFO systemInfo{};
                ::GetSystemInfo(&systemInfo);
                s_PageSize = systemInfo.dwPageSize;
                s_AllocationGranularity = systemInfo.dwAllocationGranularity;
            }
        }

        void* VirtualBuffer::ReserveMemoryRange(size_t sizeInBytes, size_t alignInBytes)
        {
            hdAssert(mem::IsAligned(s_AllocationGranularity, alignInBytes), u8"Can only allocate with alignment compatible with allocation granularity.");

            void* result = ::VirtualAlloc(NULL, sizeInBytes, MEM_RESERVE, PAGE_READWRITE);

            return result;
        }

        void VirtualBuffer::FreeMemoryRange(void* memory)
        {
            ::VirtualFree(memory, 0, MEM_RELEASE);
        }

        void VirtualBuffer::MapMemoryRange(void* memory, size_t oldSizeInBytes, size_t sizeInBytes)
        {
            size_t oldSizeInPages = mem::AlignAbove(oldSizeInBytes, s_PageSize) / s_PageSize;
            size_t newSizeInPages = mem::AlignAbove(sizeInBytes, s_PageSize) / s_PageSize;

            if (oldSizeInPages != newSizeInPages)
            {
                hdAssert(newSizeInPages > oldSizeInPages, u8"Old size is bigger than new size.");
                ::VirtualAlloc(reinterpret_cast<std::byte*>(memory) + oldSizeInPages * s_PageSize, (newSizeInPages - oldSizeInPages) * s_PageSize, MEM_COMMIT, PAGE_READWRITE);
            }
        }

        void VirtualBuffer::UnmapMemoryRange(void* memory, size_t oldSizeInBytes, size_t sizeInBytes)
        {
            size_t oldSizeInPages = mem::AlignAbove(oldSizeInBytes, s_PageSize) / s_PageSize;
            size_t newSizeInPages = mem::AlignAbove(sizeInBytes, s_PageSize) / s_PageSize;

            if (oldSizeInPages != newSizeInPages)
            {
                hdAssert(newSizeInPages < oldSizeInPages, u8"Old size is smaller than new size.");
                ::VirtualFree(reinterpret_cast<std::byte*>(memory) + newSizeInPages * s_PageSize, (oldSizeInPages - newSizeInPages) * s_PageSize, MEM_DECOMMIT);
            }
        }
    }
}

#endif
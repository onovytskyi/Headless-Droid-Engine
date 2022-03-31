#include "Config/Bootstrap.h"

#include "Framework/Memory/AllocatorConcurrentAdapter.h"

namespace hd
{
    namespace mem
    {
        AllocatorConcurrentAdaper::AllocatorConcurrentAdaper(Allocator& hostAllocator)
            : m_HostAllocator{ hostAllocator }
            , m_Lock{}
        {

        }

        AllocatorConcurrentAdaper::~AllocatorConcurrentAdaper()
        {

        }

        void* AllocatorConcurrentAdaper::Allocate(size_t size, size_t align)
        {
            std::lock_guard autoLock(m_Lock);

            return m_HostAllocator.Allocate(size, align);
        }

        void AllocatorConcurrentAdaper::Deallocate(void* memory, size_t size, size_t align)
        {
            std::lock_guard autoLock(m_Lock);

            return m_HostAllocator.Deallocate(memory, size, align);
        }
    }
}
#include "Engine/Config/Bootstrap.h"

#include "Engine/Engine/Memory/LinearAllocatorConcurrentAdapter.h"
#include "Engine/Debug/Assert.h"

namespace hd
{
    namespace mem
    {
        LinearAllocatorConcurrentAdaper::LinearAllocatorConcurrentAdaper(LinearAllocator& hostAllocator)
            : m_HostAllocator{ hostAllocator }
            , m_Lock{}
            , m_OwnerThread{ std::this_thread::get_id() }
        {

        }

        LinearAllocatorConcurrentAdaper::~LinearAllocatorConcurrentAdaper()
        {

        }

        void* LinearAllocatorConcurrentAdaper::Allocate(size_t size, size_t align)
        {
            std::lock_guard autoLock(m_Lock);

            return m_HostAllocator.Allocate(size, align);
        }

        void* LinearAllocatorConcurrentAdaper::AllocateWithOffset(size_t offset, size_t size, size_t align)
        {
            std::lock_guard autoLock(m_Lock);

            return m_HostAllocator.AllocateWithOffset(offset, size, align);
        }

        size_t LinearAllocatorConcurrentAdaper::GetMarker() const
        {
            hdAssert(m_OwnerThread == std::this_thread::get_id(), u8"GetMarker call and non owner thread probably means that allocator concurrent adapter misused.");

            return m_HostAllocator.GetMarker();
        }

        void LinearAllocatorConcurrentAdaper::Reset()
        {
            hdAssert(m_OwnerThread == std::this_thread::get_id(), u8"Reset call and non owner thread probably means that allocator concurrent adapter misused.");

            m_HostAllocator.Reset();
        }

        void LinearAllocatorConcurrentAdaper::Reset(size_t marker)
        {
            hdAssert(m_OwnerThread == std::this_thread::get_id(), u8"Reset call and non owner thread probably means that allocator concurrent adapter misused.");

            m_HostAllocator.Reset(marker);
        }

    }
}
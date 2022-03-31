#include "Config/Bootstrap.h"

#include "Foundation/Memory/FixedLinearAllocator.h"

#include "Foundation/Memory/Utils.h"

namespace hd
{
    namespace mem
    {
        FixedLinearAllocator::FixedLinearAllocator(void* memory, const size_t size) 
            : m_MemoryBegin{ static_cast<std::byte*>(memory) }
            , m_MemoryEnd{ m_MemoryBegin + size }
            , m_MemoryPointer{ m_MemoryBegin }
        {

        }

        void* FixedLinearAllocator::Allocate(const size_t size, const size_t align)
        {
            std::byte* allocation = m_MemoryPointer;
            std::byte* alignedAllocation = AlignAbove(allocation, align);

            if (alignedAllocation + size > m_MemoryEnd)
                return nullptr;

            m_MemoryPointer = alignedAllocation + size;

            return alignedAllocation;
        }

        void FixedLinearAllocator::Deallocate(void* /*memory*/, size_t /*sizeInBytes*/, size_t /*alignInBytes*/)
        {
            // NOP
        }

        size_t FixedLinearAllocator::GetMarker() const
        {
            return reinterpret_cast<size_t>(m_MemoryPointer);
        }

        void FixedLinearAllocator::Reset()
        {
            Reset(reinterpret_cast<size_t>(m_MemoryBegin));
        }

        void FixedLinearAllocator::Reset(const size_t marker)
        {
            m_MemoryPointer = reinterpret_cast<std::byte*>(marker);  // NOLINT(performance-no-int-to-ptr)
        }
    }
}
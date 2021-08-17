#include "Engine/Config/Bootstrap.h"

#include "Engine/Foundation/Memory/FixedLinearAllocator.h"

#include "Engine/Foundation/Memory/Utils.h"

namespace hd
{
    namespace mem
    {
        FixedLinearAllocator::FixedLinearAllocator(void* memory, size_t size) 
            : m_MemoryBegin{ reinterpret_cast<std::byte*>(memory) }
            , m_MemoryEnd{ m_MemoryBegin + size }
            , m_MemoryPointer{ m_MemoryBegin }
        {

        }

        void* FixedLinearAllocator::Allocate(size_t size, size_t align)
        {
            std::byte* allocation = m_MemoryPointer;
            std::byte* alignedAllocation = reinterpret_cast<std::byte*>(AlignAbove(allocation, align));

            if (alignedAllocation + size > m_MemoryEnd)
                return nullptr;

            m_MemoryPointer = alignedAllocation + size;

            return alignedAllocation;
        }

        void FixedLinearAllocator::Deallocate(void* memory, size_t sizeInBytes, size_t alignInBytes)
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

        void FixedLinearAllocator::Reset(size_t marker)
        {
            m_MemoryPointer = reinterpret_cast<std::byte*>(marker);
        }
    }
}
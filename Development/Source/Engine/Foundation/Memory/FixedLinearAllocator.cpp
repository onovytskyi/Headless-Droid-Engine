#include "Engine/Foundation/Memory/FixedLinearAllocator.h"

#include "Engine/Foundation/Memory/Utils.h"

namespace hd
{
    namespace mem
    {
        FixedLinearAllocator::FixedLinearAllocator(void* memory, size_t size) 
            : m_MemoryBegin{ reinterpret_cast<uint8_t*>(memory) }
            , m_MemoryEnd{ m_MemoryBegin + size }
            , m_MemoryPointer{ m_MemoryBegin }
        {

        }

        void* FixedLinearAllocator::Allocate(size_t size, size_t align)
        {
            return AllocateWithOffset(0, size, align);
        }

        void* FixedLinearAllocator::AllocateWithOffset(size_t offset, size_t size, size_t align)
        {
            uint8_t* allocation = m_MemoryPointer + offset;
            uint8_t* alignedAllocation = reinterpret_cast<uint8_t*>(AlignAbove(allocation, align));

            if (alignedAllocation + size > m_MemoryEnd)
                return nullptr;

            m_MemoryPointer = alignedAllocation + size;

            return alignedAllocation;
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
            m_MemoryPointer = reinterpret_cast<uint8_t*>(marker);
        }

    }
}
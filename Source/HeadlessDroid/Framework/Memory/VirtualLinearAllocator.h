#pragma once

#include "Framework/Memory/VirtualBuffer.h"

namespace hd
{
    namespace mem
    {
        class VirtualLinearAllocator : public Allocator
        {
        public:
            VirtualLinearAllocator(size_t maximumSize);
            VirtualLinearAllocator(size_t maximumSize, size_t initialSize);
            VirtualLinearAllocator(size_t maximumSize, size_t initialSize, bool autoShrink);
            ~VirtualLinearAllocator();

            hdNoncopyable(VirtualLinearAllocator)

            void* Allocate(size_t size, size_t align) override;
            void Deallocate(void* memory, size_t sizeInBytes, size_t alignInBytes) override;

            size_t GetMarker() const;

            void Reset();
            void Reset(size_t marker);

            void Shrink();

        private:
            VirtualBuffer m_Memory;
            size_t m_UsedSize;
            bool m_AutoShrink;
        };
    }
}
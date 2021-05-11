#pragma once

#include "Engine/Foundation/Memory/LinearAllocator.h"
#include "Engine/Framework/Memory/VirtualBuffer.h"

namespace hd
{
    namespace mem
    {
        class VirtualLinearAllocator : public LinearAllocator
        {
        public:
            VirtualLinearAllocator(size_t maximumSize);
            VirtualLinearAllocator(size_t maximumSize, size_t initialSize);
            VirtualLinearAllocator(size_t maximumSize, size_t initialSize, bool autoShrink);
            ~VirtualLinearAllocator();

            hdNoncopyable(VirtualLinearAllocator)

            void* Allocate(size_t size, size_t align) override;
            void* AllocateWithOffset(size_t offset, size_t size, size_t align) override;

            size_t GetMarker() const override;

            void Reset() override;
            void Reset(size_t marker) override;

            void Shrink();

        private:
            VirtualBuffer m_Memory;
            size_t m_UsedSize;
            bool m_AutoShrink;
        };
    }
}
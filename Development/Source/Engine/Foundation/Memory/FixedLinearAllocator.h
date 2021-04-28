#pragma once

#include <stdint.h>
#include "Engine/Foundation/Memory/LinearAllocator.h"

namespace hd
{
    namespace mem
    {
        class FixedLinearAllocator : public LinearAllocator
        {
        public:
            FixedLinearAllocator(void* memory, size_t size);

            void* Allocate(size_t size, size_t align) override;
            void* AllocateWithOffset(size_t offset, size_t size, size_t align) override;
            size_t GetMarker() const override;
            void Reset() override;
            void Reset(size_t marker) override;

        private:
            uint8_t* m_MemoryBegin;
            uint8_t* m_MemoryEnd;
            uint8_t* m_MemoryPointer;
        };
    }
}
#pragma once

namespace hd
{
    namespace mem
    {
        class LinearAllocator
        {
        public:
            virtual ~LinearAllocator(){}

            virtual void* Allocate(size_t size, size_t align) = 0;
            virtual void* AllocateWithOffset(size_t offset, size_t size, size_t align) = 0;
            virtual size_t GetMarker() const = 0;
            virtual void Reset() = 0;
            virtual void Reset(size_t marker) = 0;
        };
    }
}
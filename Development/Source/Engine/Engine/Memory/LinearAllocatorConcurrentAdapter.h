#pragma once

#include "Engine/Foundation/Memory/LinearAllocator.h"

namespace hd
{
    namespace mem
    {
        class LinearAllocatorConcurrentAdaper : public LinearAllocator
        {
        public:
            LinearAllocatorConcurrentAdaper(LinearAllocator& hostAllocator);
            ~LinearAllocatorConcurrentAdaper();

            void* Allocate(size_t size, size_t align) override;
            void* AllocateWithOffset(size_t offset, size_t size, size_t align) override;
            size_t GetMarker() const override;
            void Reset() override;
            void Reset(size_t marker) override;

        private:
            LinearAllocator& m_HostAllocator;
            std::mutex m_Lock;
            std::thread::id m_OwnerThread;
        };
    }
}
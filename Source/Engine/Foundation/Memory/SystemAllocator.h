#pragma once

namespace hd
{
    class SystemAllocator : public Allocator
    {
    public:
        SystemAllocator();

        hdNoncopyable(SystemAllocator)

        void* Allocate(size_t size, size_t align) override;
        void Deallocate(void* memory, size_t sizeInBytes, size_t alignInBytes) override;
    };
}
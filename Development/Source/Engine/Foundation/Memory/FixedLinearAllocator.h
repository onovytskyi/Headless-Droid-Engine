#pragma once

namespace hd
{
    namespace mem
    {
        class FixedLinearAllocator : public Allocator
        {
        public:
            FixedLinearAllocator(void* memory, size_t size);

            hdNoncopyable(FixedLinearAllocator)

            void* Allocate(size_t size, size_t align) override;
            void Deallocate(void* memory, size_t sizeInBytes, size_t alignInBytes) override;
            size_t GetMarker() const;
            void Reset();
            void Reset(size_t marker);

        private:
            std::byte* m_MemoryBegin;
            std::byte* m_MemoryEnd;
            std::byte* m_MemoryPointer;
        };
    }
}
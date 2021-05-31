#pragma once

#include "Engine/Framework/Memory/CachedPoolAllocationScope.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace util
    {
        class BestFitAllocatorHelper
        {
        public:
            static const size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

            BestFitAllocatorHelper(mem::AllocationScope& allocationScope, size_t size);

            size_t Allocate(size_t count, size_t align);
            void Deallocate(size_t offset, size_t count);

            // #HACK Remove this method as soon as possible
            // It is used to handle hacky usage of this class in HeapAllocator
            void Reset();
            bool Empty();

            size_t GetSize();

        private:
            struct Range
            {
                Range* m_NextRange;
                Range* m_PrevRange;

                size_t Offset;
                size_t Size;
            };

            mem::CachedPoolAllocationScope<Range> m_RangesAllocator;
            Range* m_FirstFreeRange;
            size_t m_Size;
        };
    }
}
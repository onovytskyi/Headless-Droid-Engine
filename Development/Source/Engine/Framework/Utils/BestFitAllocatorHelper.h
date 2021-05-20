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

            BestFitAllocatorHelper(mem::AllocationScope& allocationScope, size_t size, size_t maxRanges);

            size_t Allocate(size_t count);
            void Deallocate(size_t offset, size_t count);

            bool Empty();

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
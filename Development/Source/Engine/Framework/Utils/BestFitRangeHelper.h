#pragma once

#include "Engine/Framework/Utils/BufferArray.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace util
    {
        class BestFitRangeHelper
        {
        public:
            static const size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

            BestFitRangeHelper(mem::AllocationScope& allocationScope, size_t size, size_t maxRanges);

            size_t BindRange(size_t count);
            void UnbindRange(size_t index, size_t count);

            bool Empty();

        private:
            struct Range
            {
                size_t Start;
                size_t Count;
            };

            BufferArray<Range> m_FreeRanges;
            size_t m_Size;
        };
    }
}
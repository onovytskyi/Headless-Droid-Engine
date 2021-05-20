#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Utils/BestFitAllocatorHelper.h"

namespace hd
{
    namespace util
    {

        BestFitAllocatorHelper::BestFitAllocatorHelper(mem::AllocationScope& allocationScope, size_t size, size_t maxRanges)
            : m_RangesAllocator{ allocationScope }
            , m_FirstFreeRange{}
            , m_Size{ size }
        {
            m_FirstFreeRange = m_RangesAllocator.Allocate();
            *m_FirstFreeRange = { nullptr, nullptr,  0, size };
        }

        size_t BestFitAllocatorHelper::Allocate(size_t size)
        {
            if (m_FirstFreeRange)
            {
                Range* bestFitRange = nullptr;
                size_t bestFitRangeDiff = std::numeric_limits<size_t>::max();
                
                for (Range* range = m_FirstFreeRange; range; range = range->m_NextRange)
                {
                      // exact fit choose this and out
                    if (range->Size == size)
                    {
                        bestFitRange = range;
                        break;
                    }

                    if (range->Size > size)
                    {
                        size_t diff = range->Size - size;
                        if (diff < bestFitRangeDiff)
                        {
                            bestFitRange = range;
                            bestFitRangeDiff = diff;
                        }
                    }
                }

                if (bestFitRange)
                {
                    size_t result = bestFitRange->Offset;

                    bestFitRange->Offset += size;
                    bestFitRange->Size -= size;

                    // Remove range if it is fully occupied
                    if (bestFitRange->Size == 0)
                    {
                        if (bestFitRange->m_PrevRange)
                        {
                            bestFitRange->m_PrevRange->m_NextRange = bestFitRange->m_NextRange;
                        }

                        if (bestFitRange->m_NextRange)
                        {
                            bestFitRange->m_NextRange->m_PrevRange = bestFitRange->m_PrevRange;
                        }

                        if (bestFitRange == m_FirstFreeRange)
                        {
                            m_FirstFreeRange = bestFitRange->m_NextRange;
                        }

                        m_RangesAllocator.Free(bestFitRange);
                    }

                    return result;
                }
            }

            return INVALID_INDEX;
        }

        void BestFitAllocatorHelper::Deallocate(size_t offset, size_t size)
        {
            Range* insertNewRangeAfter = nullptr;

            if (m_FirstFreeRange)
            {
                size_t unbindBegin = offset;
                size_t unbindEnd = unbindBegin + size;

#if defined(HD_ENABLE_ASSERTS)
                // Check if range to unbind does not intersect existing free ranges
                for (Range* range = m_FirstFreeRange; range; range = range->m_NextRange)
                {
                    size_t rangeBegin = range->Offset;
                    size_t rangeEnd = rangeBegin + range->Size;

                    bool intersecting = unbindBegin < rangeEnd && unbindEnd > rangeBegin;
                    hdAssert(!intersecting, u8"Range to unbind intersecting other free range.");
                }
#endif
                for (Range* range = m_FirstFreeRange; range; range = range->m_NextRange)
                {
                    // there is no valid range to coalesce with, break out
                    if (range->Offset > (offset + size))
                    {
                        break;
                    }

                    size_t rangeBegin = range->Offset;
                    size_t rangeEnd = rangeBegin + range->Size;

                    // Check if the range to unbind is right next to the start of a free range
                    if (unbindEnd == rangeBegin)
                    {
                        range->Offset = offset;
                        range->Size += size;
                        return;
                    }

                    // Check if the range to unbind is right next to the end of a free range
                    if (unbindBegin == rangeEnd)
                    {
                        range->Size += size;

                        if (range->m_NextRange)
                        {
                            if ((range->Offset + range->Size) == range->m_NextRange->Offset)
                            {
                                Range* rangeToRemove = range->m_NextRange;
                                range->Size += rangeToRemove->Size;
                                range->m_NextRange = rangeToRemove->m_NextRange;

                                if (rangeToRemove->m_NextRange)
                                {
                                    rangeToRemove->m_NextRange->m_PrevRange = range;
                                }

                                m_RangesAllocator.Free(rangeToRemove);
                            }
                        }
                        

                        return;
                    }

                    insertNewRangeAfter = range;
                }
            }

            Range* insertNewRangeBefore = insertNewRangeAfter ? insertNewRangeAfter->m_NextRange : m_FirstFreeRange;

            Range* newRange = m_RangesAllocator.Allocate();
            *newRange = { insertNewRangeBefore, insertNewRangeAfter, offset, size };
            if (newRange->m_PrevRange)
            {
                newRange->m_PrevRange->m_NextRange = newRange;
            }

            if (newRange->m_NextRange)
            {
                newRange->m_NextRange->m_PrevRange = newRange;
            }

            if (newRange->m_NextRange == m_FirstFreeRange)
            {
                m_FirstFreeRange = newRange;
            }
        }

        bool BestFitAllocatorHelper::Empty()
        {
            return m_FirstFreeRange && m_FirstFreeRange->Offset == 0 && m_FirstFreeRange->Size == m_Size;
        }
    }
}
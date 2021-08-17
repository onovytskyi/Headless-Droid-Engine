#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Utils/BestFitAllocatorHelper.h"

#include "Engine/Debug/Assert.h"
#include "Engine/Foundation/Memory/Utils.h"

namespace hd
{
    namespace util
    {
        BestFitAllocatorHelper::BestFitAllocatorHelper(Allocator& generalAllocator, size_t size)
            : m_GeneralAllocator{ generalAllocator }
            , m_FirstFreeRange{}
            , m_Size{ size }
        {
            m_FirstFreeRange = hdNew(m_GeneralAllocator, Range){ nullptr, nullptr,  0, size };
        }

        BestFitAllocatorHelper::~BestFitAllocatorHelper()
        {
            ClearAllRanges();
        }

        size_t BestFitAllocatorHelper::Allocate(size_t size, size_t align)
        {
            if (m_FirstFreeRange)
            {
                Range* bestFitRange = nullptr;
                size_t bestFitRangeDiff = std::numeric_limits<size_t>::max();
                
                for (Range* range = m_FirstFreeRange; range; range = range->m_NextRange)
                {
                      // exact fit choose this and out
                    if (mem::IsAligned(range->Offset, align) && range->Size == size)
                    {
                        bestFitRange = range;
                        break;
                    }

                    size_t alignedOffset = mem::AlignAbove(range->Offset, align);
                    size_t adjustedSize = size + (alignedOffset - range->Offset);
                    if (range->Size >= adjustedSize)
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
                    size_t result = mem::AlignAbove(bestFitRange->Offset, align);

                    // Check if we can allocate from the very beginning of the range
                    if (result == bestFitRange->Offset)
                    {
                        bestFitRange->Offset += size;
                        bestFitRange->Size -= size;
                    }
                    // Check if allocation spans to the very end of the range
                    else if ((bestFitRange->Offset + bestFitRange->Size) == (result + size))
                    {
                        bestFitRange->Size -= size;
                    }
                    // Otherwise we should split the range in two
                    else
                    {
                        Range* secondRange = hdNew(m_GeneralAllocator, Range)();
                        secondRange->Offset = result + size;
                        secondRange->Size = (bestFitRange->Offset + bestFitRange->Size) - secondRange->Offset;
                        secondRange->m_PrevRange = bestFitRange;
                        secondRange->m_NextRange = bestFitRange->m_NextRange;
                        if (secondRange->m_NextRange)
                        {
                            secondRange->m_NextRange->m_PrevRange = secondRange;
                        }

                        bestFitRange->Size = result - bestFitRange->Offset;
                        bestFitRange->m_NextRange = secondRange;
                    }

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

                        hdSafeDelete(m_GeneralAllocator, bestFitRange);
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

                                hdSafeDelete(m_GeneralAllocator, rangeToRemove);
                            }
                        }
                        

                        return;
                    }

                    insertNewRangeAfter = range;
                }
            }

            Range* insertNewRangeBefore = insertNewRangeAfter ? insertNewRangeAfter->m_NextRange : m_FirstFreeRange;

            Range* newRange = hdNew(m_GeneralAllocator, Range){ insertNewRangeBefore, insertNewRangeAfter, offset, size };
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

        void BestFitAllocatorHelper::Reset()
        {
            ClearAllRanges();
            m_FirstFreeRange = hdNew(m_GeneralAllocator, Range) { nullptr, nullptr, 0, m_Size };
        }

        bool BestFitAllocatorHelper::Empty()
        {
            return m_FirstFreeRange && m_FirstFreeRange->Offset == 0 && m_FirstFreeRange->Size == m_Size;
        }

        size_t BestFitAllocatorHelper::GetSize()
        {
            return m_Size;
        }

        void BestFitAllocatorHelper::ClearAllRanges()
        {
            for (Range* range = m_FirstFreeRange; range; range = range->m_NextRange)
            {
                hdSafeDelete(m_GeneralAllocator, range->m_PrevRange);
            }
        }
    }
}
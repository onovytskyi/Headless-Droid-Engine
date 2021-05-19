#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Utils/BestFitRangeHelper.h"

namespace hd
{
    namespace util
    {

        BestFitRangeHelper::BestFitRangeHelper(mem::AllocationScope& allocationScope, size_t size, size_t maxRanges)
            : m_FreeRanges{ allocationScope, maxRanges }
            , m_Size{ size }
        {
            m_FreeRanges.Add({ 0, size });
        }

        size_t BestFitRangeHelper::BindRange(size_t count)
        {
            if (m_FreeRanges.GetSize() != 0)
            {
                size_t bestFitRangeIdx = INVALID_INDEX;
                size_t bestFitRangeDiff = std::numeric_limits<size_t>::max();
                for (size_t freeRangeIdx = 0; freeRangeIdx < m_FreeRanges.GetSize(); ++freeRangeIdx)
                {
                    Range& candidateFreeRange = m_FreeRanges[freeRangeIdx];

                    // exact fit choose this and out
                    if (candidateFreeRange.Count == count)
                    {
                        bestFitRangeIdx = freeRangeIdx;
                        break;
                    }

                    if (candidateFreeRange.Count > count)
                    {
                        size_t diff = candidateFreeRange.Count - count;
                        if (diff < bestFitRangeDiff)
                        {
                            bestFitRangeIdx = freeRangeIdx;
                            bestFitRangeDiff = diff;
                        }
                    }
                }

                if (bestFitRangeIdx != INVALID_INDEX)
                {
                    Range& bestFreeRange = m_FreeRanges[bestFitRangeIdx];

                    size_t result = bestFreeRange.Start;

                    bestFreeRange.Start += count;
                    bestFreeRange.Count -= count;

                    // Remove range if it is fully occupied
                    if (bestFreeRange.Count == 0)
                    {
                        m_FreeRanges.RemoveFast(bestFitRangeIdx);
                    }

                    return result;
                }
            }

            return INVALID_INDEX;
        }

        void BestFitRangeHelper::UnbindRange(size_t index, size_t count)
        {
            if (m_FreeRanges.GetSize() != 0)
            {
                size_t unbindBegin = index;
                size_t unbindEnd = unbindBegin + count;

#if defined(HD_ENABLE_ASSERTS)
                // Check if range to unbind does not intersect existing free ranges
                for (size_t freeRangeIdx = 0; freeRangeIdx < m_FreeRanges.GetSize(); ++freeRangeIdx)
                {
                    Range& range = m_FreeRanges[freeRangeIdx];

                    size_t rangeBegin = range.Start;
                    size_t rangeEnd = rangeBegin + range.Count;

                    bool intersecting = unbindBegin < rangeEnd && unbindEnd > rangeBegin;
                    hdAssert(!intersecting, u8"Range to unbind intersecting other free range.");
                }
#endif

                for (size_t freeRangeIdx = 0; freeRangeIdx < m_FreeRanges.GetSize(); ++freeRangeIdx)
                {
                    Range& candidateFreeRange = m_FreeRanges[freeRangeIdx];

                    size_t rangeBegin = candidateFreeRange.Start;
                    size_t rangeEnd = rangeBegin + candidateFreeRange.Count;

                    // Check if the range to unbind is right next to the start of a free range
                    if (unbindEnd == rangeBegin)
                    {
                        candidateFreeRange.Start = index;
                        candidateFreeRange.Count += count;
                        return;
                    }

                    // Check if the range to unbind is right next to the end of a free range
                    if (unbindBegin == rangeEnd)
                    {
                        candidateFreeRange.Count += count;
                        return;
                    }
                }
            }

            m_FreeRanges.Add({ index, count });
        }

        bool BestFitRangeHelper::Empty()
        {
            return m_FreeRanges.GetSize() == 1 && m_FreeRanges[0].Start == 0 && m_FreeRanges[0].Count == m_Size;
        }
    }
}
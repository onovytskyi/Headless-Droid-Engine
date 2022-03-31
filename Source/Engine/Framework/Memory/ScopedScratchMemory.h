#pragma once

namespace hd
{
    class ScopedScratchMemory
    {
    public:
        ScopedScratchMemory();
        ~ScopedScratchMemory();

        hdNoncopyable(ScopedScratchMemory);

    private:
        size_t m_Marker;
    };
}
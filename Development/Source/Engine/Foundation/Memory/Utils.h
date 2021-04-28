#pragma once

namespace hd
{
    namespace mem
    {
        size_t AlignBelow(size_t value, size_t alignment);
        void* AlignBelow(void* pointer, size_t alignment);

        size_t AlignAbove(size_t value, size_t alignment);
        void* AlignAbove(void* pointer, size_t alignment);
    }
}
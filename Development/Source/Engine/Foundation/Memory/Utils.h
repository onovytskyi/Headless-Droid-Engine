#pragma once

namespace hd
{
    namespace mem
    {
        inline constexpr size_t KB(size_t value) { return value * 1024U; }
        inline constexpr size_t MB(size_t value) { return KB(value) * 1024U; }
        inline constexpr size_t GB(size_t value) { return MB(value) * 1024U; }
        inline constexpr size_t TB(size_t value) { return GB(value) * 1024U; }

        inline constexpr size_t ToKB(size_t value) { return value / 1024U; }
        inline constexpr size_t ToMB(size_t value) { return ToKB(value) / 1024U; }
        inline constexpr size_t ToGB(size_t value) { return ToMB(value) / 1024U; }
        inline constexpr size_t ToTB(size_t value) { return ToGB(value) / 1024U; }

        size_t AlignBelow(size_t value, size_t alignment);
        void* AlignBelow(void* pointer, size_t alignment);

        size_t AlignAbove(size_t value, size_t alignment);
        void* AlignAbove(void* pointer, size_t alignment);
    }
}
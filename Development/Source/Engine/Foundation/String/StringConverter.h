#pragma once

namespace hd
{
    namespace str
    {
        void ToNarrow(wchar_t const* source, char8_t* dest, size_t destSize);
        void ToWide(char8_t const* source, wchar_t* dest, size_t destSize);

        size_t SizeAsNarrow(wchar_t const* source);
        size_t SizeAsWide(char8_t const* source);

        size_t LengthAsNarrow(wchar_t const* source);
        size_t LengthAsWide(char8_t const* source);
    };
}

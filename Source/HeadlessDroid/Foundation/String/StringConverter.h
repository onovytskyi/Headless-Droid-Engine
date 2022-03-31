#pragma once

namespace hd
{
    void ToNarrow(wchar_t const* source, char8_t* dest, size_t destSize);
    void ToWide(char8_t const* source, wchar_t* dest, size_t destSize);

    void ToNarrow(std::pmr::wstring const& source, std::pmr::u8string& dest);
    void ToNarrow(std::wstring const& source, std::pmr::u8string& dest);
    void ToWide(std::pmr::u8string const& source, std::pmr::wstring& dest);
    void ToWide(std::u8string const& source, std::pmr::wstring& dest);

    size_t SizeAsNarrow(wchar_t const* source);
    size_t SizeAsWide(char8_t const* source);

    size_t LengthAsNarrow(wchar_t const* source);
    size_t LengthAsWide(char8_t const* source);
}

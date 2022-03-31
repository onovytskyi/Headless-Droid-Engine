#include "Engine/Config/Bootstrap.h"

#if defined(HD_PLATFORM_WIN64)

#include "Engine/Foundation/String/StringConverter.h"

namespace hd
{
    namespace str
    {
        void ToNarrow(wchar_t const* source, char8_t* dest, size_t destSize)
        {
            WideCharToMultiByte(CP_UTF8, 0, source, -1, reinterpret_cast<char*>(dest), int(destSize) / sizeof(char8_t), nullptr, nullptr);
        }

        void ToWide(char8_t const* source, wchar_t* dest, size_t destSize)
        {
            MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(source), -1, dest, int(destSize) / sizeof(wchar_t));
        }

        size_t SizeAsNarrow(wchar_t const* source)
        {
            return LengthAsNarrow(source) * sizeof(char8_t);
        }

        size_t SizeAsWide(char8_t const* source)
        {
            return LengthAsWide(source) * sizeof(wchar_t);
        }

        size_t LengthAsNarrow(wchar_t const* source)
        {
            return size_t(WideCharToMultiByte(CP_UTF8, 0, source, -1, nullptr, 0, nullptr, nullptr));
        }

        size_t LengthAsWide(char8_t const* source)
        {
            return size_t(MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(source), -1, nullptr, 0));
        }
    }
}

#endif

#include "Config/Bootstrap.h"

#include "Foundation/String/StringConverter.h"

namespace hd
{
    namespace str
    {
        void ToNarrow(std::pmr::wstring const& source, std::pmr::u8string& dest)
        {
            size_t narrowLength = LengthAsNarrow(source.c_str()) - 1;
            dest.resize(narrowLength);
            ToNarrow(source.c_str(), dest.data(), narrowLength * sizeof(std::pmr::u8string::value_type));
        }

        void ToNarrow(std::wstring const& source, std::pmr::u8string& dest)
        {
            size_t narrowLength = LengthAsNarrow(source.c_str()) - 1;
            dest.resize(narrowLength);
            ToNarrow(source.c_str(), dest.data(), narrowLength * sizeof(std::pmr::u8string::value_type));
        }

        void ToWide(std::pmr::u8string const& source, std::pmr::wstring& dest)
        {
            size_t wideLength = LengthAsWide(source.c_str()) - 1;
            dest.resize(wideLength);
            ToWide(source.c_str(), dest.data(), wideLength * sizeof(std::pmr::wstring::value_type));
        }

        void ToWide(std::u8string const& source, std::pmr::wstring& dest)
        {
            size_t wideLength = LengthAsWide(source.c_str()) - 1;
            dest.resize(wideLength);
            ToWide(source.c_str(), dest.data(), wideLength * sizeof(std::pmr::wstring::value_type));
        }
    }
}

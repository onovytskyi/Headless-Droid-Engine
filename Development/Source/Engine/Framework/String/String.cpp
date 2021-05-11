#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/String/String.h"

#include "Engine/Foundation/String/StringConverter.h"

namespace hd
{
    namespace str
    {
        String::String(mem::AllocationScope& allocationScope)
            : m_Memory{ allocationScope }
        {

        }

        String::String(mem::AllocationScope& allocationScope, char8_t const* data)
            : m_Memory{ allocationScope }
        {
            size_t dataSize = strlen(reinterpret_cast<char const*>(data)) + 1;
            m_Memory.Resize(dataSize);

            memcpy_s(m_Memory.GetData(), m_Memory.GetSize(), data, dataSize);
        }

        String::~String()
        {

        }

        wchar_t const* String::AsWide(mem::AllocationScope& allocationScope) const
        {
            size_t wideSize = SizeAsWide(CStr());
            wchar_t* wideString = reinterpret_cast<wchar_t*>(allocationScope.AllocateMemory(wideSize, alignof(wchar_t)));
            ToWide(CStr(), wideString, wideSize);

            return wideString;
        }

        char8_t const* String::CStr() const
        {
            return m_Memory.GetDataAs<char8_t const*>();
        }

    }
}
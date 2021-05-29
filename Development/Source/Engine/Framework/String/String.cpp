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
            Assign(data);
        }

        String::~String()
        {

        }

        void String::Assign(char8_t const* data)
        {
            size_t dataSize = strlen(reinterpret_cast<char const*>(data)) + 1;
            m_Memory.Resize(dataSize);

            memcpy_s(m_Memory.GetData(), m_Memory.GetSize(), data, dataSize);
        }

        String& String::operator=(char8_t const* text)
        {
            Assign(text);

            return *this;
        }

        String& String::operator=(String const& text)
        {
            Assign(text.CStr());

            return *this;
        }

        void String::Append(char8_t const* text)
        {
            auto inputTextLength = strlen(reinterpret_cast<const char*>(text));
            if (text == nullptr || inputTextLength == 0)
            {
                return;
            }

            if (m_Memory.GetSize() == 0)
            {
                Assign(text);
            }
            else
            {
                size_t oldStringSize = m_Memory.GetSize() - 1;
                size_t newStringSize = oldStringSize + inputTextLength + 1;

                m_Memory.ResizeWithCopy(newStringSize);

                memcpy_s(m_Memory.GetData() + oldStringSize, m_Memory.GetSize() - oldStringSize, text, inputTextLength + 1);
            }
        }

        String& String::operator+(char8_t const* text)
        {
            Append(text);

            return *this;
        }

        String& String::operator+(String const& text)
        {
            Append(text.CStr());

            return *this;
        }

        bool String::operator==(String const& other)
        {
            if (m_Memory.GetSize() == other.m_Memory.GetSize())
            {
                return memcmp(m_Memory.GetData(), other.m_Memory.GetData(), m_Memory.GetSize()) == 0;
            }

            return false;
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
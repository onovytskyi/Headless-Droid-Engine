#pragma once

#include "Engine/Framework/Memory/Buffer.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace str
    {
        class String
        {
        public:
            String(mem::AllocationScope& allocationScope);
            String(mem::AllocationScope& allocationScope, char8_t const* data);
            ~String();

            wchar_t const* AsWide(mem::AllocationScope& allocationScope) const;

            char8_t const* CStr() const;

        private:
            mem::Buffer m_Memory;
        };
    }
}
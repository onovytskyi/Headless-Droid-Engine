#pragma once

#include "Engine/Framework/Memory/Buffer.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace util
    {
        template<typename T, size_t Size>
        class Array
        {
        public:
            Array(mem::AllocationScope& allocationScope);

            T& operator[](size_t index);
            T const& operator[](size_t index) const;

            size_t GetSize() const;

        private:
            mem::Buffer m_Memory;
        };
    }
}

#include "Engine/Framework/Utils/Array.hpp"
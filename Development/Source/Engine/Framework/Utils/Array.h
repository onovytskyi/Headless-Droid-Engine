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
        template<typename T>
        class Array
        {
        public:
            Array(mem::AllocationScope& allocationScope, size_t size);

            T& operator[](size_t index);
            T const& operator[](size_t index) const;

            size_t GetSize() const;

        private:
            mem::Buffer m_Memory;
        };
    }
}

#include "Engine/Framework/Utils/Array.hpp"
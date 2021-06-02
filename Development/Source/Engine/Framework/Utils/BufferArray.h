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
        class BufferArray
        {
        public:
            BufferArray(mem::AllocationScope& allocationScope, size_t size);

            T& operator[](size_t index);
            T const& operator[](size_t index) const;

            T& At(size_t index);
            T const& At(size_t index) const;

            void Add(T const& value);
            void RemoveFast(size_t index);
            void RemoveSlow(size_t index);

            void ResizeToMax();
            void Clear();

            T* GetData();
            T const* GetData() const;
            size_t GetSize() const;

            T* begin();
            T* end();

            T const* begin() const;
            T const* end() const;

        private:
            mem::Buffer m_Memory;
            size_t m_Size;
        };
    }
}

#include "Engine/Framework/Utils/BufferArray.hpp"
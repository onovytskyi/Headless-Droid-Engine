#pragma once

#include "Engine/Framework/Memory/AllocationScope.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;

        class Buffer
        {
        public:
            explicit Buffer(AllocationScope& allocationScope);
            Buffer(AllocationScope& allocator, size_t size);
            ~Buffer();

            Buffer(Buffer const& other);
            Buffer& operator=(Buffer const& other);

            Buffer(Buffer&& other) noexcept;
            Buffer& operator=(Buffer&& other) noexcept;

            void Resize(size_t size);
            void ResizeWithCopy(size_t size);

            void Assign(void* data, size_t size);

            void Clear();

            std::byte* GetData();
            std::byte const* GetData() const;
            size_t GetSize() const;

            template<typename T> T GetDataAs();
            template<typename T> const typename std::remove_pointer<T>::type* GetDataAs() const;

        private:
            AllocationScope* m_AllocationScope;
            std::byte* m_Memory;
            size_t m_Size;
        };
    }
}

#include "Engine/Framework/Memory/Buffer.hpp"

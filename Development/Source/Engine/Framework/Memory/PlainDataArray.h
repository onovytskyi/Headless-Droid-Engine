#pragma once

namespace hd
{
    template<typename T>
    class PlainDataArray
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<T>;

        explicit PlainDataArray(allocator_type allocator);
        PlainDataArray(size_t size, allocator_type allocator);
        PlainDataArray(void const* data, size_t size, allocator_type allocator);

        PlainDataArray(PlainDataArray const& other);
        PlainDataArray(PlainDataArray&& other);
        PlainDataArray(PlainDataArray const& other, allocator_type const& allocator);
        PlainDataArray(PlainDataArray&& other, allocator_type const& allocator);
        PlainDataArray& operator= (PlainDataArray const& other);
        PlainDataArray& operator= (PlainDataArray&& other);

        ~PlainDataArray();

        void Resize(size_t size);
        void Assign(void const* data, size_t size);
        void Fill(T value);

        size_t Size() const;
        bool Empty() const;
        void Clear();

        T* Data();
        T const* Data() const;

        T& operator[] (size_t index);
        T const& operator[] (size_t index) const;
        T& At(size_t index);
        T const& At(size_t index) const;

    private:
        allocator_type m_Allocator;
        T* m_Data;
        size_t m_Size;
    };
}

#include "Engine/Framework/Memory/PlainDataArray.hpp"
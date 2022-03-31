#include "Debug/Assert.h"

namespace hd
{
    template<typename T>
    inline PlainDataArray<T>::PlainDataArray(allocator_type allocator)
        : m_Allocator{ allocator }
        , m_Data{}
        , m_Size{}
    {

    }

    template<typename T>
    inline PlainDataArray<T>::PlainDataArray(size_t size, allocator_type allocator)
        : m_Allocator{ allocator }
        , m_Data{}
        , m_Size{}
    {
        Resize(size);
    }

    template<typename T>
    inline PlainDataArray<T>::PlainDataArray(void const* data, size_t size, allocator_type allocator)
        : m_Allocator{ allocator }
        , m_Data{}
        , m_Size{}
    {
        Resize(size / sizeof(T));
        memcpy_s(m_Data, m_Size * sizeof(T), data, size);
    }

    template<typename T>
    inline hd::PlainDataArray<T>::PlainDataArray(PlainDataArray const& other)
        : m_Allocator{ other.m_Allocator }
        , m_Data{}
        , m_Size{}
    {
        Resize(other.m_Size);
        memcpy_s(m_Data, m_Size * sizeof(T), other.m_Data, other.m_Size * sizeof(T));
    }

    template<typename T>
    inline hd::PlainDataArray<T>::PlainDataArray(PlainDataArray&& other)
        : m_Allocator{ other.m_Allocator }
        , m_Data{}
        , m_Size{}
    {
        std::swap(m_Data, other.m_Data);
        std::swap(m_Size, other.m_Size);
    }

    template<typename T>
    inline hd::PlainDataArray<T>::PlainDataArray(PlainDataArray const& other, allocator_type const& allocator)
        : m_Allocator{ allocator }
        , m_Data{}
        , m_Size{}
    {
        Resize(other.m_Size);
        memcpy_s(m_Data, m_Size * sizeof(T), other.m_Data, other.m_Size * sizeof(T));
    }

    template<typename T>
    inline hd::PlainDataArray<T>::PlainDataArray(PlainDataArray&& other, allocator_type const& allocator)
        : m_Allocator{ allocator }
        , m_Data{}
        , m_Size{}
    {
        if (m_Allocator == other.m_Allocator)
        {
            std::swap(m_Data, other.m_Data);
            std::swap(m_Size, other.m_Size);
        }
        else
        {
            Resize(other.m_Size);
            memcpy_s(m_Data, m_Size * sizeof(T), other.m_Data, other.m_Size * sizeof(T));
        }
    }

    template<typename T>
    inline hd::PlainDataArray<T>::~PlainDataArray()
    {
        Clear();
    }

    template<typename T>
    inline hd::PlainDataArray<T>& hd::PlainDataArray<T>::operator=(PlainDataArray const& other)
    {
        Resize(other.m_Size);
        memcpy_s(m_Data, m_Size, other.m_Data, other.m_Size);

        return *this;
    }

    template<typename T>
    inline hd::PlainDataArray<T>& hd::PlainDataArray<T>::operator=(PlainDataArray&& other)
    {
        Clear();

        if (m_Allocator == other.m_Allocator)
        {
            std::swap(m_Data, other.m_Data);
            std::swap(m_Size, other.m_Size);
        }
        else
        {
            Resize(other.m_Size);
            memcpy_s(m_Data, m_Size * sizeof(T), other.m_Data, other.m_Size * sizeof(T));
        }

        return *this;
    }

    template<typename T>
    inline void hd::PlainDataArray<T>::Resize(size_t size)
    {
        if (size != m_Size)
        {
            Clear();

            m_Size = size;
            m_Data = m_Allocator.allocate(m_Size);
        }
    }

    template<typename T>
    inline void hd::PlainDataArray<T>::Assign(void const* data, size_t size)
    {
        Resize(size / sizeof(T));
        memcpy_s(m_Data, m_Size * sizeof(T), data, size);
    }

    template<typename T>
    inline void hd::PlainDataArray<T>::Fill(T value)
    {
        for (uint32_t i = 0; i < m_Size; ++i)
        {
            m_Data[i] = value;
        }
    }

    template<typename T>
    inline size_t hd::PlainDataArray<T>::Size() const
    {
        return m_Size;
    }

    template<typename T>
    inline bool hd::PlainDataArray<T>::Empty() const
    {
        return m_Size == 0;
    }

    template<typename T>
    inline void hd::PlainDataArray<T>::Clear()
    {
        m_Allocator.deallocate(m_Data, m_Size);
        m_Data = nullptr;
        m_Size = 0;
    }

    template<typename T>
    inline T* hd::PlainDataArray<T>::Data()
    {
        return m_Data;
    }

    template<typename T>
    inline T const* hd::PlainDataArray<T>::Data() const
    {
        return m_Data;
    }

    template<typename T>
    inline T& hd::PlainDataArray<T>::operator[](size_t index)
    {
        hdAssert(index < m_Size);
        return m_Data[index];
    }

    template<typename T>
    inline T const& hd::PlainDataArray<T>::operator[](size_t index) const
    {
        hdAssert(index < m_Size);
        return m_Data[index];
    }

    template<typename T>
    inline T& hd::PlainDataArray<T>::At(size_t index)
    {
        hdAssert(index < m_Size);
        return m_Data[index];
    }

    template<typename T>
    inline T const& hd::PlainDataArray<T>::At(size_t index) const
    {
        hdAssert(index < m_Size);
        return m_Data[index];
    }
}

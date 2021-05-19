#include "Engine/Debug/Assert.h"

namespace hd
{
    namespace util
    {
        template<typename T>
        BufferArray<T>::BufferArray(mem::AllocationScope& allocationScope, size_t size)
            : m_Memory{ allocationScope, sizeof(T) * size }
        {
            memset(m_Memory.GetData(), 0, m_Memory.GetSize());
        }

        template<typename T>
        T& BufferArray<T>::operator[](size_t index)
        {
            return At(index);
        }

        template<typename T>
        T const& BufferArray<T>::operator[](size_t index) const
        {
            return At(index);
        }

        template<typename T>
        T& BufferArray<T>::At(size_t index)
        {
            hdAssert(index < m_Size, u8"Accessing index out of bounds.");
            return *(m_Memory.GetDataAs<T*>() + index);
        }

        template<typename T>
        T const& BufferArray<T>::At(size_t index) const
        {
            hdAssert(index < m_Size, u8"Accessing index out of bounds.");
            return *(m_Memory.GetDataAs<T*>() + index);
        }

        template<typename T>
        void BufferArray<T>::Add(T const& value)
        {
            hdAssert(m_Size < m_Memory.GetSize() / sizeof(T), u8"Cannot add element to array. Max array size reached.");

            m_Size += 1;
            At(m_Size - 1) = value;
        }

        template<typename T>
        void BufferArray<T>::RemoveFast(size_t index)
        {
            hdAssert(index < m_Size, u8"Accessing index out of bounds.");

            size_t lastElementIdx = GetSize() - 1;
            if (index < lastElementIdx)
            {
                std::swap(At(index), At(lastElementIdx));
            }
            m_Size -= 1;
        }

        template<typename T>
        void BufferArray<T>::RemoveSlow(size_t index)
        {
            hdAssert(index < m_Size, u8"Accessing index out of bounds.");

            size_t lastElementIdx = GetSize() - 1;
            for (size_t moveToIdx = index; moveToIdx < lastElementIdx; ++moveToIdx)
            {
                At(moveToIdx) = At(moveToIdx + 1);
            }
            m_Size -= 1;
        }

        template<typename T>
        void BufferArray<T>::ResizeToMax()
        {
            m_Size = m_Memory.GetSize() / sizeof(T);
        }

        template<typename T>
        void BufferArray<T>::Clear()
        {
            m_Size = 0;
        }

        template<typename T>
        size_t BufferArray<T>::GetSize() const
        {
            return m_Size;
        }

        template<typename T>
        T* BufferArray<T>::begin()
        {
            return m_Memory.GetDataAs<T*>();
        }

        template<typename T>
        T* BufferArray<T>::end()
        {
            return m_Memory.GetDataAs<T*>() + GetSize();
        }
    }
}

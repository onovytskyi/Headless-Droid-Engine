#include "Engine/Debug/Assert.h"

namespace hd
{
    namespace util
    {
        template<typename T>
        Array<T>::Array(mem::AllocationScope& allocationScope, size_t size)
            : m_Memory{ allocationScope, sizeof(T) * size }
        {
            memset(m_Memory.GetData(), 0, m_Memory.GetSize());
        }

        template<typename T>
        T& Array<T>::operator[](size_t index)
        {
            return *(m_Memory.GetDataAs<T*>() + index);
        }

        template<typename T>
        T const& Array<T>::operator[](size_t index) const
        {
            return *(m_Memory.GetDataAs<T*>() + index);
        }

        template<typename T>
        size_t Array<T>::GetSize() const
        {
            return m_Memory.GetSize() / sizeof(T);
        }
    }
}

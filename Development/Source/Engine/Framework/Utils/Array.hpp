#include "Engine/Debug/Assert.h"

namespace hd
{
    namespace util
    {
        template<typename T, size_t Size>
        Array<T, Size>::Array(mem::AllocationScope& allocationScope)
            : m_Memory{ allocationScope, sizeof(T) * Size }
        {
            memset(m_Memory.GetData(), 0, m_Memory.GetSize());
        }

        template<typename T, size_t Size>
        T& Array<T, Size>::operator[](size_t index)
        {
            return *(m_Memory.GetDataAs<T*>() + index);
        }

        template<typename T, size_t Size>
        T const& Array<T, Size>::operator[](size_t index) const
        {
            return *(m_Memory.GetDataAs<T*>() + index);
        }

        template<typename T, size_t Size>
        size_t Array<T, Size>::GetSize() const
        {
            return Size;
        }
    }
}

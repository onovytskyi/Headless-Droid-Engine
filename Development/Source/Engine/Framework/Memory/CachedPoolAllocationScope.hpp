#include "Engine/Debug/Assert.h"

namespace hd
{
    namespace mem
    {
        template<typename T>
        inline CachedPoolAllocationScope<T>::CachedPoolAllocationScope(AllocationScope& parentScope)
            : m_ParentScope{ parentScope }
            , m_FirstFreeObject{ nullptr }
        {
            hdAssert(sizeof(T) >= sizeof(std::byte*), u8"Cached pool allocation scope can only operate with objects of at lest % bytes size.", sizeof(std::byte*));
        }

        template<typename T>
        inline T* CachedPoolAllocationScope<T>::Allocate()
        {
            if (m_FirstFreeObject)
            {
                T* result = m_FirstFreeObject;
                T** objectAsPointer = reinterpret_cast<T**>(result);
                m_FirstFreeObject = *objectAsPointer;

                return result;
            }

            return m_ParentScope.AllocatePOD<T>();
        }

        template<typename T>
        inline void CachedPoolAllocationScope<T>::Free(T* object)
        {
            T** objectAsPointer = reinterpret_cast<T**>(object);
            *objectAsPointer = m_FirstFreeObject;
            m_FirstFreeObject = object;
        }
    }
}

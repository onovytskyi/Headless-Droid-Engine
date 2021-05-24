#include "Engine/Debug/Assert.h"

namespace hd
{
    namespace mem
    {
        template<typename T, typename... Args>
        inline T* AllocationScope::AllocateObject(Args&&... args)
        {
            void* allocation = m_Allocator.AllocateWithOffset(sizeof(Finalizer), sizeof(T), alignof(T));

            T* result = new(allocation)T(std::forward<Args>(args)...);

            hdAssert(result != nullptr, u8"Cannot allocate memory. Allocator is full.");

            Finalizer* finalizer = reinterpret_cast<Finalizer*>(allocation) - 1;
            finalizer->m_OnDestroy = &CallDestructor<T>;
            finalizer->m_NextFinalizer = m_NextFinalizer;
            m_NextFinalizer = finalizer;

            return result;
        }

        template<typename T>
        inline T* AllocationScope::AllocatePOD()
        {
            void* allocation = m_Allocator.Allocate(sizeof(T), alignof(T));
            hdAssert(allocation != nullptr, u8"Cannot allocate memory. Allocator is full.");

            T* result = new(allocation)T;

            return result;
        }

        template<typename T>
        inline T* AllocationScope::AllocatePODArray(size_t size)
        {
            void* allocation = reinterpret_cast<T*>(m_Allocator.Allocate(sizeof(T) * size, alignof(T)));
            hdAssert(allocation != nullptr, u8"Cannot allocate memory. Allocator is full.");

            T* result = reinterpret_cast<T*>(allocation);
            for (size_t podIdx = 0; podIdx < size; ++podIdx)
            {
                new(result + podIdx)T;
            }

            return result;
        }
    }
}
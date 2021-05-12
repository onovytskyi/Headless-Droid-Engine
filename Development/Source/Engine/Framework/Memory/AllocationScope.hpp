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
            T* result = new(allocation)T;

            hdAssert(result != nullptr, u8"Cannot allocate memory. Allocator is full.");

            return result;
        }
    }
}
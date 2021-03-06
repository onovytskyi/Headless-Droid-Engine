#pragma once

#include "Engine/Foundation/Memory/LinearAllocator.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope
        {
        public:
            AllocationScope(LinearAllocator& allocator);
            ~AllocationScope();

            hdNoncopyable(AllocationScope)

            template<typename T, typename... Args>
            T* AllocateObject(Args&&... args);

            template<typename T>
            T* AllocatePOD();

            template<typename T>
            T* AllocatePODArray(size_t size);

            void* AllocateMemory(size_t size, size_t align);

            void Reset();

        private:
            class Finalizer
            {
            public:
                void (*m_OnDestroy)(void* pointer);
                Finalizer* m_NextFinalizer;
            };

            template <typename T> static void CallDestructor(void* pointer) { static_cast<T*>(pointer)->~T(); }

            LinearAllocator& m_Allocator;
            size_t m_ResetMarker;
            Finalizer* m_NextFinalizer;
        };
    }
}

#include "Engine/Framework/Memory/AllocationScope.hpp"
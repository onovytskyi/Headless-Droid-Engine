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

            template<typename T, typename... Args>
            T* AllocateObject(Args... args);

            template<typename T>
            T* AllocatePOD();

            void* AllocateMemory(size_t size, size_t align);

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
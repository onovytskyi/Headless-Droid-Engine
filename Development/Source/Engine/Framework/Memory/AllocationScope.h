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

        template<typename T, typename... Args>
        T* hd::mem::AllocationScope::AllocateObject(Args... args)
        {
            void* allocation = m_Allocator.AllocateWithOffset(sizeof(Finalizer), sizeof(T), alignof(T));

            T* result = new(allocation)T(args...);

            Finalizer* finalizer = reinterpret_cast<Finalizer*>(allocation) - 1;
            finalizer->m_OnDestroy = &CallDestructor<T>;
            finalizer->m_NextFinalizer = m_NextFinalizer;
            m_NextFinalizer = finalizer;

            return result;   
        }

        template<typename T>
        T* hd::mem::AllocationScope::AllocatePOD()
        {
            void* allocation = m_Allocator.Allocate(sizeof(T), alignof(T));
            T* result = new(allocation)T;

            return result;
        }
    }
}
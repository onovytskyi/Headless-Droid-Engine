#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Memory/AllocationScope.h"

namespace hd
{
    namespace mem
    {
        AllocationScope::AllocationScope(LinearAllocator& allocator)
            : m_Allocator{ allocator }
            , m_ResetMarker{ m_Allocator.GetMarker() }
            , m_NextFinalizer{}
        {

        }

        AllocationScope::~AllocationScope()
        {
            Reset();
        }

        void* AllocationScope::AllocateMemory(size_t size, size_t align)
        {
            return m_Allocator.Allocate(size, align);
        }

        void AllocationScope::Reset()
        {
            for (Finalizer* finalizer = m_NextFinalizer; finalizer != nullptr; finalizer = finalizer->m_NextFinalizer)
            {
                void* objectPointer = finalizer + 1;
                (*finalizer->m_OnDestroy)(objectPointer);
            }
            m_Allocator.Reset(m_ResetMarker);
        }
    }
}
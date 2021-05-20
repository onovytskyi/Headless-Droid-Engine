#pragma once

#include "Engine/Framework/Memory/AllocationScope.h"

namespace hd
{
    namespace mem
    {
        template<typename T>
        class CachedPoolAllocationScope
        {
        public:
            CachedPoolAllocationScope(AllocationScope& parentScope);

            hdNoncopyable(CachedPoolAllocationScope);

            T* Allocate();
            void Free(T* object);
        private:
            AllocationScope& m_ParentScope;
            T* m_FirstFreeObject;
        };
    }
}

#include "Engine/Framework/Memory/CachedPoolAllocationScope.hpp"
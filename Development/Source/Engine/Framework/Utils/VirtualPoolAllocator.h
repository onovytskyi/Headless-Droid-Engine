#pragma once

#include "Engine/Framework/Memory/VirtualBuffer.h"

namespace hd
{
    namespace util
    {
        template<typename Payload>
        class VirtualPoolAllocator
        {
        public:
            using Handle = uint64_t;

            VirtualPoolAllocator(uint32_t maxItemCount);
            ~VirtualPoolAllocator();
            
            Handle Allocate(Payload** outItem);
            void Free(Handle handle);

            bool IsValid(Handle handle) const;
            Payload& Get(Handle handle);

        private:
            struct Item
            {
                uint32_t Version;
                union 
                {
                    Payload Data;
                    uint32_t NextFreeItem;
                };
            };

            mem::VirtualBuffer m_Memory;
            uint32_t m_FirstFreeItem;
        };
    }
}

#include "Engine/Framework/Utils/VirtualPoolAllocator.hpp"
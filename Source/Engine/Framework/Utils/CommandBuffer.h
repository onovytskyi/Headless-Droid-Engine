#pragma once

#include "Engine/Framework/Memory/VirtualBuffer.h"

namespace hd
{
    namespace util
    {
        class CommandBuffer
        {
        public:
            friend class CommandBufferReader;

            CommandBuffer(size_t maxSizeInBytes);
            ~CommandBuffer();

            hdNoncopyable(CommandBuffer)

            template<typename T>
            T& Write();

            template<typename T>
            T& Write(size_t count);

            void Clear();

            mem::VirtualBuffer& GetBuffer();

        private:
            std::byte* WriteToVirtualBuffer(size_t size);
            std::byte* ReadFromVirtualBuffer(size_t size, size_t& offset);

            mem::VirtualBuffer m_Buffer;
        };
    }
}

#include "Engine/Framework/Utils/CommandBuffer.hpp"
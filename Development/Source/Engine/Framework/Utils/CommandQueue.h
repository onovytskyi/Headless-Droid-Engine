#pragma once

#include "Engine/Framework/Memory/VirtualBuffer.h"

namespace hd
{
    namespace util
    {
        class CommandQueue
        {
        public:
            CommandQueue(size_t maxSizeInBytes);
            ~CommandQueue();

            hdNoncopyable(CommandQueue)

            template<typename Command>
            Command& PushCommand();

            std::byte* PushData(size_t size);

            uint32_t PopCommandID();

            template<typename Command>
            Command& PopCommand();

            std::byte* PopData(size_t size);

            bool HasCommands() const;

            void Clear();

        private:
            template<typename T>
            T* WriteToVirtualBuffer();

            template<typename T>
            T* ReadFromVirtualBuffer();

            std::byte* WriteToVirtualBuffer(size_t size);
            std::byte* ReadFromVirtualBuffer(size_t size);

            mem::VirtualBuffer m_Buffer;
            size_t m_ReadOffset;
        };
    }
}

#include "Engine/Framework/Utils/CommandQueue.hpp"
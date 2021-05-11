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

            template<typename T>
            T& Push();

            template<typename T>
            T& Pop();

            template<typename T>
            T& Push(size_t count);

            template<typename T>
            T& Pop(size_t count);

            bool HasCommands() const;

            void Clear();

        private:
            std::byte* WriteToVirtualBuffer(size_t size);
            std::byte* ReadFromVirtualBuffer(size_t size);

            mem::VirtualBuffer m_Buffer;
            size_t m_ReadOffset;
        };
    }
}

#include "Engine/Framework/Utils/CommandQueue.hpp"
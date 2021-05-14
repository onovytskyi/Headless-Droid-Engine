#pragma once

#include "Engine/Framework/Utils/CommandBuffer.h"

namespace hd
{
    namespace util
    {
        class CommandBufferReader
        {
        public:
            CommandBufferReader(CommandBuffer& commandBuffer);

            hdNoncopyable(CommandBufferReader)

            template<typename T>
            T& Read();

            template<typename T>
            T& Read(size_t count);

            bool HasCommands() const;

        private:
            CommandBuffer& m_CommandBuffer;
            size_t m_ReadOffset;
        };
    }
}

#include "Engine/Framework/Utils/CommandBufferReader.hpp"
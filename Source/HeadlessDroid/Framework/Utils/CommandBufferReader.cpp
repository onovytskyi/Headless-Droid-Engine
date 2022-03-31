#include "Config/Bootstrap.h"

#include "Framework/Utils/CommandBufferReader.h"

namespace hd
{
    namespace util
    {

        CommandBufferReader::CommandBufferReader(CommandBuffer& commandBuffer)
            : m_CommandBuffer{ commandBuffer }
            , m_ReadOffset{ 0 }
        {

        }

        bool CommandBufferReader::HasCommands() const
        {
            return m_ReadOffset < m_CommandBuffer.m_Buffer.GetSize();
        }

    }
}
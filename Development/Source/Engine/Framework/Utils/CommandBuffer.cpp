#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Utils/CommandBuffer.h"

#include "Engine/Debug/Assert.h"

namespace hd
{
    namespace util
    {
        CommandBuffer::CommandBuffer(size_t maxSizeInBytes)
            : m_Buffer{ maxSizeInBytes, 1 }
            , m_ReadOffset{}
        {

        }

        CommandBuffer::~CommandBuffer()
        {

        }

        bool CommandBuffer::HasCommands() const
        {
            return m_ReadOffset < m_Buffer.GetSize();
        }

        void CommandBuffer::Clear()
        {
            m_Buffer.Resize(0);
            m_ReadOffset = 0;
        }

        std::byte* CommandBuffer::WriteToVirtualBuffer(size_t size)
        {
            std::byte* memory = m_Buffer.GetData() + m_Buffer.GetSize();
            m_Buffer.Resize(m_Buffer.GetSize() + size);

            return memory;
        }

        std::byte* CommandBuffer::ReadFromVirtualBuffer(size_t size)
        {
            hdAssert(m_ReadOffset + size <= m_Buffer.GetSize(), u8"Cannot read past command queue memory.");

            std::byte* memory = m_Buffer.GetData() + m_ReadOffset;
            m_ReadOffset += size;

            return memory;
        }
    }
}
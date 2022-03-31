#include "Config/Bootstrap.h"

#include "Framework/Utils/CommandBuffer.h"

#include "Debug/Assert.h"

namespace hd
{
    namespace util
    {
        CommandBuffer::CommandBuffer(size_t maxSizeInBytes)
            : m_Buffer{ maxSizeInBytes, 1 }
        {

        }

        CommandBuffer::~CommandBuffer()
        {

        }

        void CommandBuffer::Clear()
        {
            m_Buffer.Resize(0);
        }

        mem::VirtualBuffer& CommandBuffer::GetBuffer()
        {
            return m_Buffer;
        }

        std::byte* CommandBuffer::WriteToVirtualBuffer(size_t size)
        {
            std::byte* memory = m_Buffer.GetData() + m_Buffer.GetSize();
            m_Buffer.Resize(m_Buffer.GetSize() + size);

            return memory;
        }

        std::byte* CommandBuffer::ReadFromVirtualBuffer(size_t size, size_t& offset)
        {
            hdAssert(offset + size <= m_Buffer.GetSize(), u8"Cannot read past command queue memory.");

            std::byte* memory = m_Buffer.GetData() + offset;
            offset += size;

            return memory;
        }
    }
}
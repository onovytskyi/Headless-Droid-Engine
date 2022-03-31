#include "Config/Bootstrap.h"

#include "Framework/Memory/VirtualBuffer.h"

#include "Debug/Assert.h"

namespace hd
{
    namespace mem
    {
        VirtualBuffer::VirtualBuffer(size_t maxSizeInBytes, size_t alignInBytes)
            : m_Memory{}
            , m_Size{}
            , m_MaxSize{ maxSizeInBytes }
        {
            m_Memory = reinterpret_cast<std::byte*>(ReserveMemoryRange(m_MaxSize, alignInBytes));
        }

        VirtualBuffer::VirtualBuffer(size_t maxSizeInBytes, size_t alignInBytes, size_t initialSizeInBytes)
            : m_Memory{}
            , m_Size{}
            , m_MaxSize{ maxSizeInBytes }
        {
            m_Memory = reinterpret_cast<std::byte*>(ReserveMemoryRange(m_MaxSize, alignInBytes));

            if (initialSizeInBytes > 0)
            {
                Resize(initialSizeInBytes);
            }
        }

        VirtualBuffer::~VirtualBuffer()
        {
            FreeMemoryRange(m_Memory);
        }

        VirtualBuffer::VirtualBuffer(VirtualBuffer const& other)
            : m_Memory{}
            , m_Size{}
            , m_MaxSize{ other.m_MaxSize }
        {
            m_Memory = reinterpret_cast<std::byte*>(ReserveMemoryRange(m_MaxSize, 1));

            if (other.m_Size)
            {
                Resize(other.m_Size);
            }

            memcpy_s(m_Memory, m_Size, other.m_Memory, other.m_Size);
        }

        VirtualBuffer& VirtualBuffer::operator=(VirtualBuffer const& other)
        {
            if (m_MaxSize != other.m_MaxSize)
            {
                FreeMemoryRange(m_Memory);
                m_MaxSize = other.m_MaxSize;
                m_Memory = reinterpret_cast<std::byte*>(ReserveMemoryRange(m_MaxSize, 1));
            }

            if (other.m_Size)
            {
                Resize(other.m_Size);
            }

            memcpy_s(m_Memory, m_Size, other.m_Memory, other.m_Size);

            return *this;
        }

        VirtualBuffer::VirtualBuffer(VirtualBuffer&& other) noexcept
            : m_Memory{}
            , m_Size{}
            , m_MaxSize{}
        {
            std::swap(m_Memory, other.m_Memory);
            std::swap(m_Size, other.m_Size);
            std::swap(m_MaxSize, other.m_MaxSize);
        }

        VirtualBuffer& VirtualBuffer::operator=(VirtualBuffer&& other) noexcept
        {
            FreeMemoryRange(m_Memory);

            m_Memory = nullptr;
            m_Size = 0;
            m_MaxSize = 0;

            std::swap(m_Memory, other.m_Memory);
            std::swap(m_Size, other.m_Size);
            std::swap(m_MaxSize, other.m_MaxSize);

            return *this;
        }

        void VirtualBuffer::Resize(size_t size)
        {
            hdAssert(size < m_MaxSize, u8"Cannot resize virtual array to % bytes. Max size is %", size, m_MaxSize);

            if (size > m_Size)
            {
                MapMemoryRange(m_Memory, m_Size, size);
            }
            else if (size < m_Size)
            {
                UnmapMemoryRange(m_Memory, m_Size, size);
            }

            m_Size = size;
        }

        void VirtualBuffer::Assign(void* data, size_t size)
        {
            Resize(size);
            memcpy_s(m_Memory, m_Size, data, size);
        }

        void VirtualBuffer::Clear()
        {
            Resize(0);
        }

        std::byte* VirtualBuffer::GetData()
        {
            return m_Memory;
        }

        std::byte const* VirtualBuffer::GetData() const
        {
            return m_Memory;
        }

        size_t VirtualBuffer::GetSize() const
        {
            return m_Size;
        }
    }
}

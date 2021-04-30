#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Memory/Buffer.h"

#include "Engine/Debug/Assert.h"
#include "Engine/Framework/Memory/AllocationScope.h"

namespace hd
{
    namespace mem
    {
        Buffer::Buffer(AllocationScope& allocationScope)
            : m_AllocationScope{ &allocationScope }
            , m_Memory{}
            , m_Size{}
        {

        }

        Buffer::Buffer(AllocationScope& allocationScope, size_t size)
            : m_AllocationScope{ &allocationScope }
            , m_Memory{}
            , m_Size{}
        {
            hdAssert(size > 0, u8"Cannot create buffer with the size of zero.");

            Resize(size);
        }

        Buffer::~Buffer()
        {
            Clear();
        }

        Buffer::Buffer(Buffer const& other)
            : m_AllocationScope{ other.m_AllocationScope }
            , m_Memory{}
            , m_Size{}
        {
            if (other.m_Size > 0)
            {
                Assign(other.m_Memory, other.m_Size);
            }
        }

        Buffer& Buffer::operator=(Buffer const& other)
        {
            Clear();
            m_AllocationScope = other.m_AllocationScope;
            if (other.m_Size > 0)
            {
                Assign(other.m_Memory, other.m_Size);
            }

            return *this;
        }

        Buffer::Buffer(Buffer&& other) noexcept
            : m_AllocationScope{}
            , m_Memory{}
            , m_Size{}
        {
            std::swap(m_AllocationScope, other.m_AllocationScope);
            std::swap(m_Memory, other.m_Memory);
            std::swap(m_Size, other.m_Size);
        }

        Buffer& Buffer::operator=(Buffer&& other) noexcept
        {
            Clear();

            m_AllocationScope = other.m_AllocationScope;
            std::swap(m_Memory, other.m_Memory);
            std::swap(m_Size, other.m_Size);

            return *this;
        }

        void Buffer::Resize(size_t size)
        {
            std::byte* allocation = reinterpret_cast<std::byte*>(m_AllocationScope->AllocateMemory(size, 1));

            Clear();

            m_Memory = allocation;
            m_Size = size;
        }

        void Buffer::ResizeWithCopy(size_t size)
        {
            std::byte* allocation = reinterpret_cast<std::byte*>(m_AllocationScope->AllocateMemory(size, 1));
            memcpy_s(allocation, size, m_Memory, m_Size);

            Clear();

            m_Memory = allocation;
            m_Size = size;
        }

        void Buffer::Clear()
        {
            m_Memory = nullptr;
            m_Size = 0;
        }

        void Buffer::Assign(void* data, size_t size)
        {
            if (m_Size != size)
            {
                Resize(size);
            }

            memcpy_s(m_Memory, m_Size, data, size);
        }

        std::byte* Buffer::GetData()
        {
            hdAssert(m_Memory != nullptr);

            return m_Memory;
        }

        std::byte const* Buffer::GetData() const
        {
            hdAssert(m_Memory != nullptr);

            return m_Memory;
        }

        size_t Buffer::GetSize() const
        {
            return m_Size;
        }
    }
}
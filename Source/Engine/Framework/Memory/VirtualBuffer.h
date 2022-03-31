#pragma once

#include "Engine/Framework/Memory/Win64/VirtualBufferWin64.h"

namespace hd
{
    namespace mem
    {
        // #TODO make unified interface between this class and mem::Buffer class
        class VirtualBuffer : public VirtualBufferPlatform
        {
        public:
            VirtualBuffer(size_t maxSizeInBytes, size_t alignInBytes);
            VirtualBuffer(size_t maxSizeInBytes, size_t alignInBytes, size_t initialSizeInBytes);
            ~VirtualBuffer();

            VirtualBuffer(VirtualBuffer const& other);
            VirtualBuffer& operator=(VirtualBuffer const& other);

            VirtualBuffer(VirtualBuffer&& other) noexcept;
            VirtualBuffer& operator=(VirtualBuffer&& other) noexcept;

            void Resize(size_t size);

            void Assign(void* data, size_t size);

            void Clear();

            std::byte* GetData();
            std::byte const* GetData() const;
            size_t GetSize() const;

            template<typename T> T GetDataAs();
            template<typename T> typename std::remove_pointer<T>::type const* GetDataAs() const;

        private:
            void* ReserveMemoryRange(size_t sizeInBytes, size_t alignInBytes);
            void FreeMemoryRange(void* memory);

            void MapMemoryRange(void* memory, size_t oldSizeInBytes, size_t sizeInBytes);
            void UnmapMemoryRange(void* memory, size_t oldSizeInBytes, size_t sizeInBytes);

            std::byte* m_Memory;
            size_t m_Size;
            size_t m_MaxSize;
        };
    }
}

#include "Engine/Framework/Memory/VirtualBuffer.hpp"

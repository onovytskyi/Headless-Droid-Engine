#pragma once

#include "Engine/Framework/Graphics/DX12/QueueDX12.h"

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    class Allocator;

    namespace util
    {
        class CommandBuffer;
    }

    namespace gfx
    {
        class Device;
        class Fence;

        class Queue : public QueuePlatform
        {
        public:
            Queue(Allocator& persistentAllocator, Device& device, QueueType type);
            ~Queue();

            hdNoncopyable(Queue)

            void Signal(Fence& fence, uint64_t value);

            void Submit(util::CommandBuffer& commandBuffer);

            void Flush();

        private:
            Allocator& m_PersistentAllocator;

            QueueType m_Type;
            uint64_t m_LastFlushValue;
            Fence* m_FlushFence;
        };
    }
}
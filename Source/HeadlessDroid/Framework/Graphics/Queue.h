#pragma once

#include "Framework/Graphics/DX12/QueueDX12.h"

#include "Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    class Allocator;
	class CommandBuffer;
	class Device;
	class Fence;

    class Queue : public QueuePlatform
    {
    public:
        Queue(Allocator& persistentAllocator, Device& device, QueueType type);
        ~Queue();

        hdNoncopyable(Queue)

        void Signal(Fence& fence, uint64_t value);

        void Submit(CommandBuffer& commandBuffer);

        void Flush();

    private:
        Allocator& m_PersistentAllocator;

        QueueType m_Type;
        uint64_t m_LastFlushValue;
        Fence* m_FlushFence;
    };
}
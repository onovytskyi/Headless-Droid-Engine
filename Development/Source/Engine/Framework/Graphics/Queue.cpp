#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/Queue.h"

#include "Engine/Framework/Graphics/Fence.h"
#include "Engine/Framework/Memory/AllocationScope.h"

namespace hd
{
    namespace gfx
    {
        Queue::Queue(Device& device, QueueType type, mem::AllocationScope& allocationScope)
            : QueuePlatform{ device, type }
            , m_Type{ type }
            , m_LastFlushValue{ 0 }
            , m_FlushFence{}
        {
            m_FlushFence = allocationScope.AllocateObject<Fence>(device, m_LastFlushValue);
        }

        Queue::~Queue()
        {

        }

        void Queue::Flush()
        {
            m_LastFlushValue += 1;
            Signal(*m_FlushFence, m_LastFlushValue);
            m_FlushFence->Wait(m_LastFlushValue);
        }

    }
}
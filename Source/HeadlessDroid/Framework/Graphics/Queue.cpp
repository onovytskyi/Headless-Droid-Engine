#include "Config/Bootstrap.h"

#include "Framework/Graphics/Queue.h"

#include "Foundation/Memory/Utils.h"
#include "Framework/Graphics/Fence.h"

namespace hd
{
    namespace gfx
    {
        Queue::Queue(Allocator& persistentAllocator, Device& device, QueueType type)
            : QueuePlatform{ device, type }
            , m_PersistentAllocator{ persistentAllocator }
            , m_Type{ type }
            , m_LastFlushValue{ 0 }
            , m_FlushFence{}
        {
            m_FlushFence = hdNew(m_PersistentAllocator, Fence)(device, m_LastFlushValue);
        }

        Queue::~Queue()
        {
            hdSafeDelete(m_PersistentAllocator, m_FlushFence);
        }

        void Queue::Flush()
        {
            m_LastFlushValue += 1;
            Signal(*m_FlushFence, m_LastFlushValue);
            m_FlushFence->Wait(m_LastFlushValue);
        }

    }
}
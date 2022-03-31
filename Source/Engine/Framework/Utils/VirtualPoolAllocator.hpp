#include "Engine/Debug/Assert.h"

namespace hd
{
    namespace util
    {
        static const uint32_t INVALID_ITEM_IDX = std::numeric_limits<uint32_t>::max();

        inline constexpr uint64_t EncodeHandle(uint32_t version, uint32_t idx)
        {
            return (uint64_t(version) << 32) | uint64_t(idx);
        }

        inline void DecodeHandle(uint64_t handle, uint32_t& version, uint32_t& idx)
        {
            version = (handle >> 32) & 0xFFFFFFFF;
            idx = handle & 0xFFFFFFFF;
        }

        template<typename Payload>
        inline VirtualPoolAllocator<Payload>::VirtualPoolAllocator(uint32_t maxItemCount)
            : m_Memory{ sizeof(Item) * maxItemCount, alignof(Item) }
            , m_FirstFreeItem{ INVALID_ITEM_IDX }
        {

        }

        template<typename Payload>
        inline VirtualPoolAllocator<Payload>::~VirtualPoolAllocator()
        {

        }

        template<typename Payload>
        inline typename VirtualPoolAllocator<Payload>::Handle VirtualPoolAllocator<Payload>::Allocate(Payload** outItem)
        {
            Handle result = InvalidHandle();
            Item* itemsArray = reinterpret_cast<Item*>(m_Memory.GetData());

            if (m_FirstFreeItem == INVALID_ITEM_IDX)
            {
                uint32_t itemIdx = uint32_t(m_Memory.GetSize() / sizeof(Item));
                m_Memory.Resize((size_t(itemIdx) + 1) * sizeof(Item));

                Item* item = itemsArray + itemIdx;
                item->Version = 1;

                if (outItem)
                {
                    *outItem = &item->Data;
                }

                result = EncodeHandle(item->Version, itemIdx);
            }
            else
            {
                uint32_t itemIdx = m_FirstFreeItem;

                Item* item = itemsArray + itemIdx;
                m_FirstFreeItem = item->NextFreeItem;

                if (outItem)
                {
                    *outItem = &item->Data;
                }

                result = EncodeHandle(item->Version, itemIdx);
            }

            return result;
        }

        template<typename Payload>
        inline void VirtualPoolAllocator<Payload>::Free(Handle handle)
        {
            uint32_t version{};
            uint32_t itemIdx{};
            DecodeHandle(handle, version, itemIdx);

            hdAssert(itemIdx < (m_Memory.GetSize() / sizeof(Item)), u8"Invalid handle. Index is outside of range.");

            Item* itemsArray = reinterpret_cast<Item*>(m_Memory.GetData());
            Item* item = itemsArray + itemIdx;

            hdAssert(item->Version == version, u8"Handle is pointing to already deleted object.");

            item->Version += 1;
            item->NextFreeItem = m_FirstFreeItem;
            m_FirstFreeItem = itemIdx;
        }

        template<typename Payload>
        inline bool VirtualPoolAllocator<Payload>::IsValid(Handle handle) const
        {
            uint32_t version{};
            uint32_t itemIdx{};
            DecodeHandle(handle, version, itemIdx);

            if (itemIdx >= (m_Memory.GetSize() / sizeof(Item)))
            {
                return false;
            }

            Item const* itemsArray = reinterpret_cast<Item const*>(m_Memory.GetData());
            Item const* item = itemsArray + itemIdx;

            if (item->Version != version)
            {
                return false;
            }

            return true;
        }

        template<typename Payload>
        inline Payload& VirtualPoolAllocator<Payload>::Get(Handle handle)
        {
            uint32_t version{};
            uint32_t itemIdx{};
            DecodeHandle(handle, version, itemIdx);

            hdAssert(itemIdx < (m_Memory.GetSize() / sizeof(Item)), u8"Invalid handle. Index is outside of range.");

            Item* itemsArray = reinterpret_cast<Item*>(m_Memory.GetData());
            Item* item = itemsArray + itemIdx;

            hdAssert(item->Version == version, u8"Handle is pointing to already deleted object.");

            return item->Data;
        }

        template<typename Payload>
        inline constexpr typename VirtualPoolAllocator<Payload>::Handle hd::util::VirtualPoolAllocator<Payload>::InvalidHandle()
        {
            return EncodeHandle(0, INVALID_ITEM_IDX);
        }
    }
}
#include "Engine/Debug/Assert.h"

namespace hd
{
    namespace gfx
    {
        template<D3D12_COMMAND_LIST_TYPE CommandListType, uint32_t MaxLists, uint32_t MaxAllocators>
        inline CommandListManager<CommandListType, MaxLists, MaxAllocators>::CommandListManager(DevicePlatform& device, mem::AllocationScope& allocationScope)
            : m_OwnerDevice{ &device }
            , m_CommandLists{ allocationScope }
            , m_CommandListsInUse{ 0 }
            , m_CommandAllocators{ allocationScope }
            , m_CommandAllocatorsInUse{ 0 }
        {

        }

        template<D3D12_COMMAND_LIST_TYPE CommandListType, uint32_t MaxLists, uint32_t MaxAllocators>
        inline CommandListManager<CommandListType, MaxLists, MaxAllocators>::~CommandListManager()
        {
            for (uint32_t commandListIdx = 0; commandListIdx < m_CommandLists.GetSize(); ++commandListIdx)
            {
                if (m_CommandLists[commandListIdx])
                {
                    m_CommandLists[commandListIdx]->Release();
                }
            }

            for (uint32_t allocatorIdx = 0; allocatorIdx < m_CommandAllocators.GetSize(); ++allocatorIdx)
            {
                if (m_CommandAllocators[allocatorIdx].Allocator)
                {
                    m_CommandAllocators[allocatorIdx].Allocator->Release();
                }
            }
        }

        template<D3D12_COMMAND_LIST_TYPE CommandListType, uint32_t MaxLists, uint32_t MaxAllocators>
        inline ID3D12GraphicsCommandList* CommandListManager<CommandListType, MaxLists, MaxAllocators>::GetCommandList(ID3D12PipelineState* pipelineState)
        {
            hdAssert(m_CommandListsInUse < m_CommandLists.GetSize(), u8"All command lists are in use. Consider increase lists count.");

            ID3D12CommandAllocator* commandAllocator = GetCommandAllocator();

            ID3D12GraphicsCommandList*& result = m_CommandLists[m_CommandListsInUse];
            m_CommandListsInUse += 1;

            if (result == nullptr)
            {
                hdEnsure(m_OwnerDevice->GetNativeDevice()->CreateCommandList(0, CommandListType, commandAllocator, pipelineState, IID_PPV_ARGS(&result)));
            }
            else
            {
                result->Reset(commandAllocator, pipelineState);
            }

            return result;
        }

        template<D3D12_COMMAND_LIST_TYPE CommandListType, uint32_t MaxLists, uint32_t MaxAllocators>
        inline void CommandListManager<CommandListType, MaxLists, MaxAllocators>::FreeCommandList(ID3D12GraphicsCommandList* commandList)
        {
            hdAssert(m_CommandListsInUse > 0, u8"Cannot free command list. No command lists are in use.");

            uint32_t swapWithIdx = m_CommandListsInUse;
            for (uint32_t commandListIdx = 0; commandListIdx < m_CommandListsInUse; ++commandListIdx)
            {
                if (m_CommandLists[commandListIdx] == commandList)
                {
                    if (commandListIdx != swapWithIdx)
                    {
                        std::swap(m_CommandLists[commandListIdx], m_CommandLists[swapWithIdx]);
                    }

                    m_CommandListsInUse -= 1;
                    return;
                }
            }

            hdAssert(false, u8"Cannot free comand list. No such entry exists in this command list manager.");
        }

        template<D3D12_COMMAND_LIST_TYPE CommandListType, uint32_t MaxLists, uint32_t MaxAllocators>
        inline void CommandListManager<CommandListType, MaxLists, MaxAllocators>::RecycleResources(uint64_t currentMarker, uint64_t completedMarker)
        {
            m_CommandListsInUse = 0;

            for (uint32_t allocatorIdx = 0; allocatorIdx < m_CommandAllocatorsInUse; ++allocatorIdx)
            {
                CommandAllocatorHolder& holder = m_CommandAllocators[allocatorIdx];

                // Check if command allocator was used this frame and set it as pending with current maker
                if (holder.Thread != std::thread::id{})
                {
                    holder.Thread = std::thread::id{};
                    holder.Marker = currentMarker;
                }

                // Check if command allocator can be reused
                if (holder.Marker != 0)
                {
                    if (holder.Marker <= completedMarker)
                    {
                        holder.Marker = 0;

                        // Move free allocator after allocators in use
                        std::swap(m_CommandAllocators[allocatorIdx], m_CommandAllocators[m_CommandAllocatorsInUse - 1]);
                        m_CommandAllocatorsInUse -= 1;
                    }
                }
            }
        }

        template<D3D12_COMMAND_LIST_TYPE CommandListType, uint32_t MaxLists, uint32_t MaxAllocators>
        inline ID3D12CommandAllocator* CommandListManager<CommandListType, MaxLists, MaxAllocators>::GetCommandAllocator()
        {
            std::thread::id currentThread = std::this_thread::get_id();

            CommandAllocatorHolder* allocatorToUse = nullptr;

            for (uint32_t allocatorIdx = 0; allocatorIdx < m_CommandAllocatorsInUse; ++allocatorIdx)
            {
                CommandAllocatorHolder& holder = m_CommandAllocators[allocatorIdx];

                // Check if allocator is not pending and used for current thread
                if (holder.Marker == 0 && holder.Thread == currentThread)
                {
                    allocatorToUse = &holder;
                    break;
                }
            }

            if (allocatorToUse == nullptr)
            {
                hdAssert(m_CommandAllocatorsInUse < m_CommandAllocators.GetSize(), u8"All command allocators are in use. Consider increase allocators count.");
                CommandAllocatorHolder& holder = m_CommandAllocators[m_CommandAllocatorsInUse];
                m_CommandAllocatorsInUse += 1;
                allocatorToUse = &holder;
                allocatorToUse->Thread = currentThread;
            }

            if (allocatorToUse->Allocator == nullptr)
            {
                hdEnsure(m_OwnerDevice->GetNativeDevice()->CreateCommandAllocator(CommandListType, IID_PPV_ARGS(&allocatorToUse->Allocator)));
            }

            return allocatorToUse->Allocator;
        }
    }
}
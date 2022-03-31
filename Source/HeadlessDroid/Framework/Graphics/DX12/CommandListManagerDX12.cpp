#include "Config/Bootstrap.h"

#include "Framework/Graphics/DX12/CommandListManagerDX12.h"

#include "Debug/Assert.h"
#include "Framework/Graphics/Device.h"

namespace hd
{
    namespace gfx
    {
        CommandListManager::CommandListManager(Allocator& generalAllocator, DevicePlatform& device, D3D12_COMMAND_LIST_TYPE type)
            : m_GeneralAllocator{ generalAllocator }
            , m_OwnerDevice{ &device }
            , m_Type{ type }
            , m_CommandLists{ &m_GeneralAllocator }
            , m_CommandListsInUse{ 0 }
            , m_CommandAllocators{ &m_GeneralAllocator }
        {
            m_CommandLists.reserve(16);
            m_CommandAllocators.reserve(16);
        }

        CommandListManager::~CommandListManager()
        {
            for (uint32_t commandListIdx = 0; commandListIdx < m_CommandLists.size(); ++commandListIdx)
            {
                if (m_CommandLists[commandListIdx])
                {
                    m_CommandLists[commandListIdx]->Release();
                }
            }

            for (uint32_t allocatorIdx = 0; allocatorIdx < m_CommandAllocators.size(); ++allocatorIdx)
            {
                if (m_CommandAllocators[allocatorIdx].Allocator)
                {
                    m_CommandAllocators[allocatorIdx].Allocator->Release();
                }
            }
        }

        ID3D12GraphicsCommandList* CommandListManager::GetCommandList(ID3D12PipelineState* pipelineState)
        {
            ID3D12CommandAllocator* commandAllocator = GetCommandAllocator();

            ID3D12GraphicsCommandList* result{};
            if (m_CommandListsInUse == m_CommandLists.size())
            {
                hdEnsure(m_OwnerDevice->GetNativeDevice()->CreateCommandList(0, m_Type, commandAllocator, pipelineState, IID_PPV_ARGS(&result)));
                m_CommandLists.push_back(result);
            }
            else
            {
                result = m_CommandLists[m_CommandListsInUse];
                result->Reset(commandAllocator, pipelineState);
            }
            m_CommandListsInUse += 1;

            return result;
        }

        void CommandListManager::FreeCommandList(ID3D12GraphicsCommandList* commandList)
        {
            hdAssert(m_CommandListsInUse > 0, u8"Cannot free command list. No command lists are in use.");

            uint32_t swapWithIdx = m_CommandListsInUse - 1;
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

        void CommandListManager::RecycleResources(uint64_t currentMarker, uint64_t completedMarker)
        {
            m_CommandListsInUse = 0;

            for (uint32_t allocatorIdx = 0; allocatorIdx < m_CommandAllocators.size(); ++allocatorIdx)
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

                        //Reset allocator
                        hdEnsure(holder.Allocator->Reset());
                    }
                }
            }
        }

        ID3D12CommandAllocator* CommandListManager::GetCommandAllocator()
        {
            std::thread::id currentThread = std::this_thread::get_id();

            for (uint32_t allocatorIdx = 0; allocatorIdx < m_CommandAllocators.size(); ++allocatorIdx)
            {
                CommandAllocatorHolder& holder = m_CommandAllocators[allocatorIdx];

                // Check if allocator is not pending and used for current thread
                if (holder.Marker == 0 && holder.Thread == currentThread)
                {
                    return holder.Allocator;
                }
            }

            for (uint32_t allocatorIdx = 0; allocatorIdx < m_CommandAllocators.size(); ++allocatorIdx)
            {
                CommandAllocatorHolder& holder = m_CommandAllocators[allocatorIdx];

                // Find free allocator
                if (holder.Marker == 0 && holder.Thread == std::thread::id{})
                {
                    holder.Thread = currentThread;
                    return holder.Allocator;
                }
            }

            CommandAllocatorHolder holder{};
            hdEnsure(m_OwnerDevice->GetNativeDevice()->CreateCommandAllocator(m_Type, IID_PPV_ARGS(&holder.Allocator)));
            holder.Thread = currentThread;

            m_CommandAllocators.push_back(holder);

            return holder.Allocator;
        }
    }
}
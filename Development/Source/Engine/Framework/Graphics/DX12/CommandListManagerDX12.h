#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Utils/Array.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace gfx
    {
        class DevicePlatform;

        template<D3D12_COMMAND_LIST_TYPE CommandListType, uint32_t MaxLists, uint32_t MaxAllocators>
        class CommandListManager
        {
        public:
            CommandListManager(DevicePlatform& device, mem::AllocationScope& allocationScope);
            ~CommandListManager();

            hdNoncopyable(CommandListManager)

            ID3D12GraphicsCommandList* GetCommandList(ID3D12PipelineState* pipelineState);
            void FreeCommandList(ID3D12GraphicsCommandList* commandList);
            void RecycleResources(uint64_t currentMarker, uint64_t completedMarker);

        private:
            ID3D12CommandAllocator* GetCommandAllocator();

            DevicePlatform* m_OwnerDevice;

            util::Array<ID3D12GraphicsCommandList*, MaxLists> m_CommandLists;
            uint32_t m_CommandListsInUse;

            struct CommandAllocatorHolder
            {
                ID3D12CommandAllocator* Allocator;
                uint64_t Marker;
                std::thread::id Thread;
            };

            util::Array<CommandAllocatorHolder, MaxAllocators> m_CommandAllocators;
            uint64_t m_CommandAllocatorsInUse;
        };
    }
}

#include "Engine/Framework/Graphics/DX12/CommandListManagerDX12.hpp"

#endif
#pragma once

#if defined(HD_GRAPHICS_API_DX12)

namespace hd
{
    class Allocator;
	class DevicePlatform;

    class CommandListManager
    {
    public:
        CommandListManager(Allocator& generalAllocator, DevicePlatform& device, D3D12_COMMAND_LIST_TYPE type);
        ~CommandListManager();

        hdNoncopyable(CommandListManager)

        ID3D12GraphicsCommandList* GetCommandList(ID3D12PipelineState* pipelineState);
        void FreeCommandList(ID3D12GraphicsCommandList* commandList);
        void RecycleResources(uint64_t currentMarker, uint64_t completedMarker);

    private:
        ID3D12CommandAllocator* GetCommandAllocator();

        Allocator& m_GeneralAllocator;

        DevicePlatform* m_OwnerDevice;
        D3D12_COMMAND_LIST_TYPE m_Type;

        std::pmr::vector<ID3D12GraphicsCommandList*> m_CommandLists;
        uint32_t m_CommandListsInUse;

        struct CommandAllocatorHolder
        {
            ID3D12CommandAllocator* Allocator;
            uint64_t Marker;
            std::thread::id Thread;
        };
        std::pmr::vector<CommandAllocatorHolder> m_CommandAllocators;
    };
}

#endif
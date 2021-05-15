#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/Device.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Debug/Assert.h"
#include "Engine/Debug/Log.h"
#include "Engine/Framework/Graphics/Backend.h"
#include "Engine/Framework/Graphics/DX12/TextureDX12.h"
#include "Engine/Framework/Graphics/GraphicCommands.h"
#include "Engine/Framework/Graphics/Queue.h"
#include "Engine/Framework/Utils/CommandBufferReader.h"

namespace hd
{
    namespace gfx
    {
        DevicePlatform::DevicePlatform(Backend& backend, mem::AllocationScope& allocationScope)
            : m_Backend{ &backend }
            , m_MessageCallbackCookie{}
            , m_GraphicsCommandListManager{}
            , m_ComputeCommandListManager{}
            , m_CopyCommandListManager{}
            , m_ResourceStateTracker{}
            , m_DescriptorManager{}
        {
            m_Adapter = backend.GetBestAdapter();

            hdEnsure(::D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf())));

#if defined(HD_ENABLE_GFX_DEBUG)
            dbg::SetDebugDevice(m_Device.Get());
            ComPtr<ID3D12InfoQueue1> infoQueue1;
            if (SUCCEEDED(m_Device.As(&infoQueue1)))
            {
                infoQueue1->RegisterMessageCallback(dbg::D3D12DebugMessageCallback, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, nullptr, &m_MessageCallbackCookie);
            }
            else
            {
                ComPtr<ID3D12InfoQueue> infoQueue;
                if (SUCCEEDED(m_Device.As(&infoQueue)))
                {
                    hdEnsure(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE));
                    hdEnsure(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE));
                    hdEnsure(infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE));

                    hdLogInfo(u8"DirectX break on warnings and errors enabled.");
                }
                else
                {
                    hdLogWarning(u8"Failed to enable breaks on DirectX warnings and errors.");
                }
            }
#endif

            m_GraphicsCommandListManager = allocationScope.AllocateObject<CommandListManager<D3D12_COMMAND_LIST_TYPE_DIRECT, 16, 24>>(*this, allocationScope);
            m_ComputeCommandListManager = allocationScope.AllocateObject<CommandListManager<D3D12_COMMAND_LIST_TYPE_COMPUTE, 16, 24>>(*this, allocationScope);
            m_CopyCommandListManager = allocationScope.AllocateObject<CommandListManager<D3D12_COMMAND_LIST_TYPE_COPY, 16, 24>>(*this, allocationScope);
            m_ResourceStateTracker = allocationScope.AllocateObject<ResourceStateTracker>(allocationScope);
            m_DescriptorManager = allocationScope.AllocateObject<DescriptorManager>(*this, allocationScope);
        }

        DevicePlatform::~DevicePlatform()
        {
#if defined(HD_ENABLE_GFX_DEBUG)
            dbg::SetDebugDevice(nullptr);

            ComPtr<ID3D12InfoQueue1> infoQueue1;
            if (SUCCEEDED(m_Device.As(&infoQueue1)))
            {
                infoQueue1->UnregisterMessageCallback(m_MessageCallbackCookie);
            }
#endif
        }

        ID3D12Device* DevicePlatform::GetNativeDevice() const
        {
            return m_Device.Get();
        }

        TextureHandle DevicePlatform::RegisterTexture(ID3D12Resource* resource, D3D12_RESOURCE_STATES state, GraphicFormat format, uint32_t flags)
        {
            Texture* texture{};
            TextureHandle result = TextureHandle(m_Backend->GetTextureAllocator().Allocate(&texture));

            new(texture)Texture(*static_cast<Device*>(this), resource, state, format, flags);

            return result;
        }

        void DevicePlatform::PresentOnQueue(Queue& queue, TextureHandle framebuffer)
        {
            Texture& framebufferTexture = m_Backend->GetTextureAllocator().Get(uint64_t(framebuffer));

            // #TODO #Optimization Minor optimization possible to prevent allocating separate command list for a single barrier
            ID3D12GraphicsCommandList* commandList = m_GraphicsCommandListManager->GetCommandList(nullptr);

            m_ResourceStateTracker->RequestTransition(framebufferTexture.GetStateTrackedData(), D3D12_RESOURCE_STATE_PRESENT);
            m_ResourceStateTracker->ApplyTransitions(*commandList);

            commandList->Close();

            ID3D12CommandList* listsToExecute[] = { commandList };
            queue.GetNativeQueue()->ExecuteCommandLists(1, listsToExecute);

            m_GraphicsCommandListManager->FreeCommandList(commandList);
        }

        void DevicePlatform::SubmitToQueue(Queue& queue, util::CommandBuffer& commandBuffer)
        {
            // #TODO #HACK As of right now we're submitting commands to queue right away, but this will not work for multithreaded recording.
            util::CommandBufferReader commandBufferReader{ commandBuffer };

            ID3D12GraphicsCommandList* commandList = m_GraphicsCommandListManager->GetCommandList(nullptr);

            while (commandBufferReader.HasCommands())
            {
                GraphicCommandType& commandType = commandBufferReader.Read<GraphicCommandType>();

                switch (commandType)
                {

                case GraphicCommandType::ClearRenderTarget:
                {
                     ClearRenderTargetCommandCommand& command = ClearRenderTargetCommandCommand::ReadFrom(commandBufferReader);
                     Texture& target = m_Backend->GetTextureAllocator().Get(uint64_t(command.Tagert));

                     m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_RENDER_TARGET);
                     m_ResourceStateTracker->ApplyTransitions(*commandList);
                     commandList->ClearRenderTargetView(target.GetRTV().HandleCPU, command.Color.data(), 0, nullptr);
                }
                break;
                

                default:
                    hdAssert(false, u8"Cannot process command. Unknown command type.");
                    break;
                }
            }

            commandList->Close();

            ID3D12CommandList* listsToExecute[] = { commandList };
            queue.GetNativeQueue()->ExecuteCommandLists(1, listsToExecute);

            m_GraphicsCommandListManager->FreeCommandList(commandList);
        }

        DescriptorManager& DevicePlatform::GetDescriptorManager()
        {
            return *m_DescriptorManager;
        }

        TextureHandle Device::CreateTexture(uint64_t width, uint32_t height, uint16_t depth, uint16_t mipLevels, GraphicFormat format, uint32_t flags, TextureDimenstion dimension,
            float clearValue[4])
        {
            hdAssert(false, u8"Not implemented.");

            return TextureHandle{};
        }

        void Device::DestroyTexture(TextureHandle handle)
        {
            hdAssert(false, u8"Not implemented.");
        }

        void Device::DestroyTextureImmediate(TextureHandle handle)
        {
            util::VirtualPoolAllocator<Texture>& textureAllocator = m_Backend->GetTextureAllocator();

            Texture& texture = textureAllocator.Get(uint64_t(handle));
            texture.GetNativeResource()->Release();
            texture.FreeDescriptors(*this);
            textureAllocator.Free(uint64_t(handle));
        }

        void Device::RecycleResources(uint64_t currentMarker, uint64_t completedMarker)
        {
            m_GraphicsCommandListManager->RecycleResources(currentMarker, completedMarker);
            m_ComputeCommandListManager->RecycleResources(currentMarker, completedMarker);
            m_CopyCommandListManager->RecycleResources(currentMarker, completedMarker);
        }
    }
}

#endif
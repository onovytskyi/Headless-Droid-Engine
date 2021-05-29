#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/Device.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Debug/Assert.h"
#include "Engine/Debug/Log.h"
#include "Engine/Framework/Graphics/Backend.h"
#include "Engine/Framework/Graphics/DX12/TextureDX12.h"
#include "Engine/Framework/Graphics/DX12/UtilsDX12.h"
#include "Engine/Framework/Graphics/GraphicCommands.h"
#include "Engine/Framework/Graphics/Queue.h"
#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/Memory/FrameworkMemoryInterface.h"
#include "Engine/Framework/Utils/CommandBufferReader.h"

namespace hd
{
    namespace gfx
    {
        DevicePlatform::DevicePlatform(Backend& backend, mem::AllocationScope& allocationScope)
            : m_Backend{ &backend }
#if defined(HD_ENABLE_GFX_DEBUG)
            , m_MessageCallbackCookie{}
#endif
            , m_GraphicsCommandListManager{}
            , m_ComputeCommandListManager{}
            , m_CopyCommandListManager{}
            , m_ResourceStateTracker{}
            , m_DescriptorManager{}
            , m_HeapAllocator{}
            , m_BuffersToFree{ allocationScope, cfg::MaxBuffersToFreeInQueue() }
            , m_TexturesToFree{ allocationScope, cfg::MaxTexturesToFreeInQueue() }
            , m_RecentBufferToFree{ allocationScope, cfg::MaxBuffersToFreePerFrame() }
            , m_RecentTexturesToFree{ allocationScope, cfg::MaxTexturesToFreePerFrame() }
        {
            m_Adapter = backend.GetBestAdapter();

            ComPtr<ID3D12Device> device;
            hdEnsure(::D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device.GetAddressOf())));
            hdEnsure(device.As(&m_Device));

            D3D12_FEATURE_DATA_D3D12_OPTIONS optionalFeatures{};
            hdEnsure(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &optionalFeatures, sizeof(optionalFeatures)));
            hdEnsure(optionalFeatures.ResourceBindingTier == D3D12_RESOURCE_BINDING_TIER_3, u8"GPU does not support Binding Tier 3.");

            D3D12_FEATURE_DATA_SHADER_MODEL shaderModel{};
            shaderModel.HighestShaderModel = D3D_SHADER_MODEL_6_6;
            hdEnsure(m_Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)));
            hdEnsure(shaderModel.HighestShaderModel >= D3D_SHADER_MODEL_6_6, u8"GPU does not support Shader Model 6.6 or higher.");

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
            CreateUnifiedRootSignature();

            m_GraphicsCommandListManager = allocationScope.AllocateObject<CommandListManager>(*this, allocationScope, D3D12_COMMAND_LIST_TYPE_DIRECT, cfg::MaxGraphicsCommandLists(), 
                cfg::MaxGraphicsCommandAllocators());
            m_ComputeCommandListManager = allocationScope.AllocateObject<CommandListManager>(*this, allocationScope, D3D12_COMMAND_LIST_TYPE_COMPUTE, cfg::MaxComputeCommandLists(),
                cfg::MaxComputeCommandAllocators());
            m_CopyCommandListManager = allocationScope.AllocateObject<CommandListManager>(*this, allocationScope, D3D12_COMMAND_LIST_TYPE_COPY, cfg::MaxCopyCommandLists(),
                cfg::MaxCopyCommandAllocators());
            m_ResourceStateTracker = allocationScope.AllocateObject<ResourceStateTracker>(allocationScope);
            m_DescriptorManager = allocationScope.AllocateObject<DescriptorManager>(*this, allocationScope);
            m_HeapAllocator = allocationScope.AllocateObject<HeapAllocator>(*static_cast<Device*>(this), allocationScope, cfg::GPUHeapSize(), cfg::MaxGPUHeaps(), cfg::KeepHeapsAliveForFrames());
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

        ID3D12Device2* DevicePlatform::GetNativeDevice() const
        {
            return m_Device.Get();
        }

        ID3D12RootSignature* DevicePlatform::GetNativeRootSignature() const
        {
            return m_RootSignature.Get();
        }

        TextureHandle DevicePlatform::RegisterTexture(ID3D12Resource* resource, D3D12_RESOURCE_STATES state, GraphicFormat format, uint32_t flags, TextureDimenstion dimension)
        {
            Texture* texture{};
            TextureHandle result = TextureHandle(m_Backend->GetTextureAllocator().Allocate(&texture));

            new(texture)Texture(*static_cast<Device*>(this), resource, state, format, flags, dimension);

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
            mem::AllocationScope scratchScope{ mem::GetScratchAllocator() };

            // #TODO #HACK As of right now we're submitting commands to queue right away, but this will not work for multithreaded recording.
            util::CommandBufferReader commandBufferReader{ commandBuffer };

            ID3D12GraphicsCommandList* commandList = m_GraphicsCommandListManager->GetCommandList(nullptr);

            while (commandBufferReader.HasCommands())
            {
                GraphicCommandType& commandType = commandBufferReader.Read<GraphicCommandType>();

                switch (commandType)
                {

                // -------------------------------------------------------------------------------------------------------------
                case GraphicCommandType::ClearRenderTarget:
                {
                     ClearRenderTargetCommand& command = ClearRenderTargetCommand::ReadFrom(commandBufferReader);
                     Texture& target = m_Backend->GetTextureAllocator().Get(uint64_t(command.Target));

                     m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_RENDER_TARGET);
                     m_ResourceStateTracker->ApplyTransitions(*commandList);
                     commandList->ClearRenderTargetView(target.GetRTV().HandleCPU, command.Color.data(), 0, nullptr);
                }
                break;

                // -------------------------------------------------------------------------------------------------------------
                case GraphicCommandType::UpdateTexture:
                {
                    UpdateTextureCommand& command = UpdateTextureCommand::ReadFrom(commandBufferReader);
                    Texture& target = m_Backend->GetTextureAllocator().Get(uint64_t(command.Target));

                    uint32_t firstSubresource = command.FirstSubresource;
                    uint32_t numSubresources = command.NumSubresources == ALL_SUBRESOURCES ? target.GetSubresourceCount() - command.FirstSubresource : command.NumSubresources;

                    D3D12_PLACED_SUBRESOURCE_FOOTPRINT* copyableFootprints = scratchScope.AllocatePODArray<D3D12_PLACED_SUBRESOURCE_FOOTPRINT>(numSubresources);
                    uint64_t totalSizeInBytes{};
                    D3D12_RESOURCE_DESC resouceDesc = target.GetNativeResource()->GetDesc();
                    m_Device->GetCopyableFootprints(&resouceDesc, firstSubresource, numSubresources, 0, copyableFootprints, nullptr, nullptr, &totalSizeInBytes);

                    HeapAllocator::Allocation uploadAllocation = m_HeapAllocator->Allocate(totalSizeInBytes, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT, D3D12_HEAP_TYPE_UPLOAD,
                        D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS, HeapAllocator::Usage::Transient);

                    std::byte* sourceMemory = command.Data;
                    std::byte* uploadMemory = reinterpret_cast<std::byte*>(uploadAllocation.CPUMappedMemory);

                    if (IsBlockCompressed(target.GetFormat()))
                    {
                        size_t blockSize = GetBlockSize(target.GetFormat());
                        for (uint32_t subresourceIdx = 0; subresourceIdx < numSubresources; ++subresourceIdx)
                        {
                            uint64_t uplodRowPitch = copyableFootprints[subresourceIdx].Footprint.RowPitch;
                            uint64_t dataRowPitch = std::max(1ULL, ((target.GetWidth() >> (firstSubresource + subresourceIdx)) + 3) / 4ULL) * blockSize;
                            uint32_t heightInBlocks = std::max(1U, ((target.GetHeight() >> (firstSubresource + subresourceIdx)) + 3) / 4U);

                            if (uplodRowPitch == dataRowPitch)
                            {
                                size_t sizeToCopy = dataRowPitch * heightInBlocks;
                                std::byte* subresourceUploadMemory = uploadMemory + copyableFootprints[subresourceIdx].Offset;

                                memcpy_s(subresourceUploadMemory, sizeToCopy, sourceMemory, sizeToCopy);
                                sourceMemory += sizeToCopy;
                            }
                            else
                            {
                                size_t rowSizeToCopy = std::min(uplodRowPitch, dataRowPitch);
                                std::byte* subresourceUploadMemory = uploadMemory + copyableFootprints[subresourceIdx].Offset;

                                for (uint32_t rowIdx = 0; rowIdx < heightInBlocks; ++rowIdx)
                                {
                                    memcpy_s(subresourceUploadMemory, rowSizeToCopy, sourceMemory, rowSizeToCopy);
                                    subresourceUploadMemory += uplodRowPitch;
                                    sourceMemory += dataRowPitch;
                                }
                            }
                        }
                    }
                    else
                    {
                        size_t formatBPP = BytesPerElement(target.GetFormat());

                        for (uint32_t subresourceIdx = 0; subresourceIdx < numSubresources; ++subresourceIdx)
                        {
                            uint64_t uplodRowPitch = copyableFootprints[subresourceIdx].Footprint.RowPitch;
                            uint64_t dataRowPitch = std::max(1ULL, (target.GetWidth() >> (firstSubresource + subresourceIdx))) * formatBPP;
                            uint32_t dataHeight = std::max(1U, (target.GetHeight() >> (firstSubresource + subresourceIdx)));

                            if (uplodRowPitch == dataRowPitch)
                            {
                                size_t sizeToCopy = dataRowPitch * dataHeight;
                                std::byte* subresourceUploadMemory = uploadMemory + copyableFootprints[subresourceIdx].Offset;

                                memcpy_s(uploadMemory, sizeToCopy, sourceMemory, sizeToCopy);
                                sourceMemory += sizeToCopy;
                            }
                            else
                            {
                                size_t rowSizeToCopy = std::min(uplodRowPitch, dataRowPitch);
                                std::byte* subresourceUploadMemory = uploadMemory + copyableFootprints[subresourceIdx].Offset;

                                for (uint32_t rowIdx = 0; rowIdx < dataHeight; ++rowIdx)
                                {
                                    memcpy_s(uploadMemory, rowSizeToCopy, sourceMemory, rowSizeToCopy);
                                    subresourceUploadMemory += uplodRowPitch;
                                    sourceMemory += dataRowPitch;
                                }
                            }
                        }
                    }
                    m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_COPY_DEST);
                    m_ResourceStateTracker->ApplyTransitions(*commandList);

                    for (uint32_t subresourceIdx = 0; subresourceIdx < numSubresources; ++subresourceIdx)
                    {
                        copyableFootprints[subresourceIdx].Offset += uploadAllocation.Offset;

                        D3D12_TEXTURE_COPY_LOCATION copySource{};
                        copySource.pResource = uploadAllocation.ResourceData->Resource;
                        copySource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
                        copySource.PlacedFootprint = copyableFootprints[subresourceIdx];

                        D3D12_TEXTURE_COPY_LOCATION copyDest{};
                        copyDest.pResource = target.GetNativeResource();
                        copyDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
                        copyDest.SubresourceIndex = firstSubresource + subresourceIdx;

                        // #TODO Implement region texture copies
                        commandList->CopyTextureRegion(&copyDest, 0, 0, 0, &copySource, nullptr);
                    }

                    m_HeapAllocator->Free(uploadAllocation);
                }
                break;
                

                default:
                    hdAssert(false, u8"Cannot process command. Unknown command type.");
                    break;
                }
            }

            hdEnsure(commandList->Close());

            ID3D12CommandList* listsToExecute[] = { commandList };
            queue.GetNativeQueue()->ExecuteCommandLists(1, listsToExecute);

            m_GraphicsCommandListManager->FreeCommandList(commandList);
        }

        DescriptorManager& DevicePlatform::GetDescriptorManager()
        {
            return *m_DescriptorManager;
        }

        HeapAllocator& DevicePlatform::GetHeapAllocator()
        {
            return *m_HeapAllocator;
        }

        void DevicePlatform::CreateUnifiedRootSignature()
        {
            mem::AllocationScope scratchScope{ mem::GetScratchAllocator() };

            const uint32_t numRootParameters = 1;
            D3D12_ROOT_PARAMETER1* rootParameters = scratchScope.AllocatePODArray<D3D12_ROOT_PARAMETER1>(numRootParameters);

            rootParameters[0] = {};
            rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
            rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
            rootParameters[0].Constants.Num32BitValues = 32;
            rootParameters[0].Constants.ShaderRegister = 0;
            rootParameters[0].Constants.RegisterSpace = 0;

            const uint32_t numStaticSamplers = 3;
            D3D12_STATIC_SAMPLER_DESC* staticSamplers = scratchScope.AllocatePODArray<D3D12_STATIC_SAMPLER_DESC>(numStaticSamplers);

            staticSamplers[0] = {};
            staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
            staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            staticSamplers[0].MinLOD = 0.0f;
            staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
            staticSamplers[0].ShaderRegister = 0;
            staticSamplers[0].RegisterSpace = 0;
            staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

            staticSamplers[1] = {};
            staticSamplers[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
            staticSamplers[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            staticSamplers[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            staticSamplers[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            staticSamplers[1].MinLOD = 0.0f;
            staticSamplers[1].MaxLOD = D3D12_FLOAT32_MAX;
            staticSamplers[1].ShaderRegister = 1;
            staticSamplers[1].RegisterSpace = 0;
            staticSamplers[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

            staticSamplers[2] = {};
            staticSamplers[2].Filter = D3D12_FILTER_ANISOTROPIC;
            staticSamplers[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            staticSamplers[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            staticSamplers[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            staticSamplers[2].MinLOD = 0.0f;
            staticSamplers[2].MaxLOD = D3D12_FLOAT32_MAX;
            staticSamplers[2].MaxAnisotropy = 16;
            staticSamplers[2].ShaderRegister = 2;
            staticSamplers[2].RegisterSpace = 0;
            staticSamplers[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

            D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc{};
            rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
            rootSignatureDesc.Desc_1_1.NumParameters = numRootParameters;
            rootSignatureDesc.Desc_1_1.pParameters = rootParameters;
            rootSignatureDesc.Desc_1_1.NumStaticSamplers = numStaticSamplers;
            rootSignatureDesc.Desc_1_1.pStaticSamplers = staticSamplers;
            rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAGS(D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED);

            ComPtr<ID3DBlob> signatureBlob;
            ComPtr<ID3DBlob> errorBlob;
            HRESULT result = ::D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signatureBlob, &errorBlob);
            if (FAILED(result))
            {
                hdEnsure(result, u8"Failed to create root signature. Error message: %", reinterpret_cast<char8_t*>(errorBlob->GetBufferPointer()));
            }

            hdEnsure(m_Device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
        }

        TextureHandle Device::CreateTexture(uint64_t width, uint32_t height, uint16_t depth, uint16_t mipLevels, GraphicFormat format, uint32_t flags, TextureDimenstion dimension,
            float clearValue[4])
        {
            bool isCube = dimension == TextureDimenstion::TextureCube;

            D3D12_RESOURCE_DESC textureDesc{};
            textureDesc.Format = ConvertToResourceFormat(format);
            textureDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
            textureDesc.Width = width;
            textureDesc.Height = height;
            textureDesc.DepthOrArraySize = isCube ? 6 : depth;
            textureDesc.MipLevels = mipLevels;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            textureDesc.Dimension = ConvertToResourceDimension(dimension);

            if (flags & (uint32_t)TextureFlags::RenderTarget)
                textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

            if (flags & (uint32_t)TextureFlags::DepthStencil)
                textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

            if (flags & (uint32_t)TextureFlags::UnorderedAccess)
                textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

            if (!(flags & (uint32_t)TextureFlags::ShaderResource))
                textureDesc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

            bool isRenderTarget = (flags & (uint32_t)TextureFlags::RenderTarget) | (flags & (uint32_t)TextureFlags::DepthStencil);

            hdAssert(!(isRenderTarget && isCube), u8"Cubemaps as render targets are not supported.");

            D3D12_CLEAR_VALUE textureClearValue{};
            if (clearValue != nullptr)
            {
                if (flags & (uint32_t)TextureFlags::RenderTarget)
                {
                    textureClearValue.Format = ConvertToWriteableFormat(format);
                    textureClearValue.Color[0] = clearValue[0];
                    textureClearValue.Color[1] = clearValue[1];
                    textureClearValue.Color[2] = clearValue[2];
                    textureClearValue.Color[3] = clearValue[3];
                }
                else if (flags & (uint32_t)TextureFlags::DepthStencil)
                {
                    textureClearValue.Format = ConvertToWriteableFormat(format);
                    textureClearValue.DepthStencil.Depth = clearValue[0];
                    textureClearValue.DepthStencil.Stencil = uint8_t(clearValue[1]);
                }
            }

            D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
            if (flags & (uint32_t)TextureFlags::RenderTarget)
            {
                initialState = D3D12_RESOURCE_STATE_RENDER_TARGET;
            }
            else if (flags & (uint32_t)TextureFlags::DepthStencil)
            {
                initialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
            }

            D3D12_HEAP_FLAGS heapFlags = isRenderTarget ? D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES : D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;

            D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = m_Device->GetResourceAllocationInfo(0, 1, &textureDesc);

            HeapAllocator::Allocation allocation = m_HeapAllocator->Allocate(allocationInfo.SizeInBytes, allocationInfo.Alignment, D3D12_HEAP_TYPE_DEFAULT, heapFlags, 
                HeapAllocator::Usage::Transient);

            hdEnsure(allocation.IsValid(), u8"Cannot allocate GPU memory for resource.");

            ID3D12Resource* textureResource{};
            m_Device->CreatePlacedResource(allocation.GPUHeap, allocation.Offset, &textureDesc, initialState, isRenderTarget ? &textureClearValue : nullptr, IID_PPV_ARGS(&textureResource));

            Texture* texture{};
            TextureHandle result = TextureHandle(m_Backend->GetTextureAllocator().Allocate(&texture));

            new(texture)Texture(*static_cast<Device*>(this), textureResource, allocation, initialState, format, flags, dimension);

            return result;
        }

        void Device::DestroyTexture(TextureHandle handle)
        {
            if (m_Backend->GetTextureAllocator().IsValid(uint64_t(handle)))
            {
                m_RecentTexturesToFree.Add(handle);
            }
        }

        void Device::DestroyTextureImmediate(TextureHandle handle)
        {
            if (m_Backend->GetTextureAllocator().IsValid(uint64_t(handle)))
            {
                util::VirtualPoolAllocator<Texture>& textureAllocator = m_Backend->GetTextureAllocator();

                Texture& texture = textureAllocator.Get(uint64_t(handle));
                texture.Free(*this);
                textureAllocator.Free(uint64_t(handle));
            }
        }

        void Device::RecycleResources(uint64_t currentMarker, uint64_t completedMarker)
        {
            m_GraphicsCommandListManager->RecycleResources(currentMarker, completedMarker);
            m_ComputeCommandListManager->RecycleResources(currentMarker, completedMarker);
            m_CopyCommandListManager->RecycleResources(currentMarker, completedMarker);
            m_HeapAllocator->RecycleResources(currentMarker, completedMarker);

            for (BufferHandle& buffer : m_RecentBufferToFree)
            {
                m_BuffersToFree.Add({ currentMarker, buffer });
            }
            m_RecentBufferToFree.Clear();

            for (TextureHandle& texture : m_RecentTexturesToFree)
            {
                m_TexturesToFree.Add({ currentMarker, texture });
            }
            m_RecentTexturesToFree.Clear();

            util::VirtualPoolAllocator<Buffer>& bufferAllocator = m_Backend->GetBufferAllocator();
            for (uint32_t bufferIdx = 0; bufferIdx < m_BuffersToFree.GetSize(); ++bufferIdx)
            {
                ResourceHolder<BufferHandle>& holder = m_BuffersToFree[bufferIdx];

                if (holder.FrameMarker <= completedMarker)
                {
                    Buffer& texture = bufferAllocator.Get(uint64_t(holder.Resource));
                    texture.Free(*this);
                    bufferAllocator.Free(uint64_t(holder.Resource));

                    m_BuffersToFree.RemoveFast(bufferIdx);
                    bufferIdx -= 1;
                }
            }

            util::VirtualPoolAllocator<Texture>& textureAllocator = m_Backend->GetTextureAllocator();
            for (uint32_t textureIdx = 0; textureIdx < m_TexturesToFree.GetSize(); ++textureIdx)
            {
                ResourceHolder<TextureHandle>& holder = m_TexturesToFree[textureIdx];

                if (holder.FrameMarker <= completedMarker)
                {
                    Texture& texture = textureAllocator.Get(uint64_t(holder.Resource));
                    texture.Free(*this);
                    textureAllocator.Free(uint64_t(holder.Resource));

                    m_TexturesToFree.RemoveFast(textureIdx);
                    textureIdx -= 1;
                }
            }
        }
    }
}

#endif
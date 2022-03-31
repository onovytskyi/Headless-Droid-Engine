#include "Config/Bootstrap.h"

#include "Framework/Graphics/Device.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Debug/Assert.h"
#include "Debug/Log.h"
#include "Foundation/Memory/Utils.h"
#include "Framework/Graphics/Backend.h"
#include "Framework/Graphics/DX12/ShaderManagerDX12.h"
#include "Framework/Graphics/DX12/TextureDX12.h"
#include "Framework/Graphics/DX12/UtilsDX12.h"
#include "Framework/Graphics/DX12/VolatileStateTrackerDX12.h"
#include "Framework/Graphics/GraphicCommands.h"
#include "Framework/Graphics/Queue.h"
#include "Framework/Graphics/RenderState.h"
#include "Framework/Memory/FrameworkMemoryInterface.h"
#include "Framework/Utils/CommandBufferReader.h"

namespace hd
{
	DevicePlatform::DevicePlatform(Allocator& persistentAllocator, Allocator& generalAllocator, Backend& backend)
		: m_PersistentAllocator{ persistentAllocator }
		  , m_GeneralAllocator{ generalAllocator }
		  , m_Backend{ &backend }
#if defined(HD_ENABLE_GFX_DEBUG)
		  , m_MessageCallbackCookie{}
#endif
		  , m_GraphicsCommandListManager{}
		  , m_ComputeCommandListManager{}
		  , m_CopyCommandListManager{}
		  , m_ResourceStateTracker{}
		  , m_DescriptorManager{}
		  , m_HeapAllocator{}
		  , m_BuffersToFree{ &m_GeneralAllocator }
		  , m_TexturesToFree{ &m_GeneralAllocator }
		  , m_RecentBuffersToFree{ &m_GeneralAllocator }
		  , m_RecentTexturesToFree{ &m_GeneralAllocator }
#if defined(HD_ENABLE_RESOURCE_COOKING)
		  , m_RenderStatesToRebuild{ &m_GeneralAllocator }
#endif
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
		SetDebugDevice(m_Device.Get());
		ComPtr<ID3D12InfoQueue1> infoQueue1;
		if (SUCCEEDED(m_Device.As(&infoQueue1)))
		{
			infoQueue1->RegisterMessageCallback(D3D12DebugMessageCallback, D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS, nullptr, &m_MessageCallbackCookie);
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

		m_GraphicsCommandListManager = hdNew(m_PersistentAllocator, CommandListManager)(m_GeneralAllocator , *this, D3D12_COMMAND_LIST_TYPE_DIRECT);
		m_ComputeCommandListManager = hdNew(m_PersistentAllocator, CommandListManager)(m_GeneralAllocator, *this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
		m_CopyCommandListManager = hdNew(m_PersistentAllocator, CommandListManager)(m_GeneralAllocator, *this, D3D12_COMMAND_LIST_TYPE_COPY);
		m_ResourceStateTracker = hdNew(m_PersistentAllocator, ResourceStateTracker)(m_GeneralAllocator);
		m_DescriptorManager = hdNew(m_PersistentAllocator, DescriptorManager)(m_GeneralAllocator, *this);
		m_HeapAllocator = hdNew(m_PersistentAllocator, HeapAllocator)(m_GeneralAllocator, *static_cast<Device*>(this), cfg::GPUHeapSize(), cfg::KeepHeapsAliveForFrames());
        
		m_BuffersToFree.reserve(64);
		m_TexturesToFree.reserve(64);
		m_RecentBuffersToFree.reserve(32);
		m_RecentTexturesToFree.reserve(32);
		m_RenderStatesToRebuild.reserve(32);
	}

	DevicePlatform::~DevicePlatform()
	{
#if defined(HD_ENABLE_GFX_DEBUG)
		SetDebugDevice(nullptr);

		ComPtr<ID3D12InfoQueue1> infoQueue1;
		if (SUCCEEDED(m_Device.As(&infoQueue1)))
		{
			infoQueue1->UnregisterMessageCallback(m_MessageCallbackCookie);
		}
#endif

		hdDelete(m_PersistentAllocator, m_GraphicsCommandListManager);
		hdDelete(m_PersistentAllocator, m_ComputeCommandListManager);
		hdDelete(m_PersistentAllocator, m_CopyCommandListManager);
		hdDelete(m_PersistentAllocator, m_ResourceStateTracker);
		hdDelete(m_PersistentAllocator, m_DescriptorManager);
		hdDelete(m_PersistentAllocator, m_HeapAllocator);
	}

	ID3D12Device2* DevicePlatform::GetNativeDevice() const
	{
		return m_Device.Get();
	}

	ID3D12RootSignature* DevicePlatform::GetNativeRootSignature() const
	{
		return m_RootSignature.Get();
	}

	TextureHandle DevicePlatform::RegisterTexture(ID3D12Resource* resource, D3D12_RESOURCE_STATES state, GraphicFormat format, TextureFlags flags, TextureDimenstion dimension)
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

	void DevicePlatform::SubmitToQueue(Queue& queue, CommandBuffer& commandBuffer)
	{
		ScopedScratchMemory scopedScratch{};

		// #TODO #HACK As of right now we're submitting commands to queue right away, but this will not work for multithreaded recording.
		CommandBufferReader commandBufferReader{ commandBuffer };

		ID3D12GraphicsCommandList* commandList = m_GraphicsCommandListManager->GetCommandList(nullptr);

		ID3D12DescriptorHeap* bindlessHeaps[2] = { m_DescriptorManager->GetResourceHeap(), m_DescriptorManager->GetSamplerHeap() };
		commandList->SetDescriptorHeaps(_countof(bindlessHeaps), bindlessHeaps);

		VolatileStateTracker* volatileState = hdNew(Scratch(), VolatileStateTracker)();
		volatileState->SetRootSignature(m_RootSignature.Get());

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
			case GraphicCommandType::ClearDepthStencil:
				{
					ClearDepthStencilCommand& command = ClearDepthStencilCommand::ReadFrom(commandBufferReader);
					Texture& target = m_Backend->GetTextureAllocator().Get(uint64_t(command.DepthStencil));

					m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
					m_ResourceStateTracker->ApplyTransitions(*commandList);
					commandList->ClearDepthStencilView(target.GetDSV().HandleCPU, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, command.Depth, command.Stencil, 0, nullptr);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::UpdateBuffer:
				{
					UpdateBufferCommand& command = UpdateBufferCommand::ReadFrom(commandBufferReader);
					GfxBuffer& target = m_Backend->GetBufferAllocator().Get(uint64_t(command.Target));

					HeapAllocator::Allocation uploadAllocation = m_HeapAllocator->Allocate(command.Size, D3D12_STANDARD_MAXIMUM_ELEMENT_ALIGNMENT_BYTE_MULTIPLE, D3D12_HEAP_TYPE_UPLOAD,
						D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS, HeapAllocator::Usage::Transient);

					memcpy_s(uploadAllocation.CPUMappedMemory, uploadAllocation.Size, command.Data, command.Size);

					m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_COPY_DEST);
					m_ResourceStateTracker->ApplyTransitions(*commandList);

					commandList->CopyBufferRegion(target.GetNativeResource(), target.GetBaseOffset() + command.Offset, uploadAllocation.ResourceData->Resource, uploadAllocation.Offset, 
					                              uploadAllocation.Size);

					m_HeapAllocator->Free(uploadAllocation);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::UpdateTexture:
				{
					UpdateTextureCommand& command = UpdateTextureCommand::ReadFrom(commandBufferReader);
					Texture& target = m_Backend->GetTextureAllocator().Get(uint64_t(command.Target));

					uint32_t firstSubresource = command.FirstSubresource;
					uint32_t numSubresources = command.NumSubresources == ALL_SUBRESOURCES ? target.GetSubresourceCount() - command.FirstSubresource : command.NumSubresources;

					D3D12_PLACED_SUBRESOURCE_FOOTPRINT* copyableFootprints = hdAllocate(Scratch(), D3D12_PLACED_SUBRESOURCE_FOOTPRINT, numSubresources);
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
                

			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::SetRenderState:
				{
					SetRenderStateCommand& command = SetRenderStateCommand::ReadFrom(commandBufferReader);

					command.State->Compile(*static_cast<Device*>(this));
					volatileState->SetRenderState(command.State);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::SetTopologyType:
				{
					SetTopologyTypeCommand& command = SetTopologyTypeCommand::ReadFrom(commandBufferReader);

					volatileState->SetTopologyType(command.Type);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::DrawInstanced:
				{
					DrawInstancedCommand& command = DrawInstancedCommand::ReadFrom(commandBufferReader);

					m_ResourceStateTracker->ApplyTransitions(*commandList);
					volatileState->ApplyChangedStatesForGrahics(*commandList);

					commandList->DrawInstanced(command.VertexCount, command.InstanceCount, 0, 0);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::SetViewports:
				{
					SetViewportsCommand& command = SetViewportsCommand::ReadFrom(commandBufferReader);

					D3D12_VIEWPORT* viewports = hdAllocate(Scratch(), D3D12_VIEWPORT, command.Count);

					for (uint32_t viewportIdx = 0; viewportIdx < command.Count; ++viewportIdx)
					{
						viewports[viewportIdx].TopLeftX = command.Viewports[viewportIdx].MinX;
						viewports[viewportIdx].Width = command.Viewports[viewportIdx].MaxX - command.Viewports[viewportIdx].MinX;

						viewports[viewportIdx].TopLeftY = command.Viewports[viewportIdx].MinY;
						viewports[viewportIdx].Height = command.Viewports[viewportIdx].MaxY - command.Viewports[viewportIdx].MinY;

						viewports[viewportIdx].MinDepth = command.Viewports[viewportIdx].MinZ;
						viewports[viewportIdx].MaxDepth = command.Viewports[viewportIdx].MaxZ;
					}

					commandList->RSSetViewports(command.Count, viewports);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::SetScissorRects:
				{
					SetScissorRectsCommand& command = SetScissorRectsCommand::ReadFrom(commandBufferReader);

					D3D12_RECT* rects = hdAllocate(Scratch(), D3D12_RECT, command.Count);

					for (uint32_t rectIdx = 0; rectIdx < command.Count; ++rectIdx)
					{
						rects[rectIdx].left = command.Rects[rectIdx].MinX;
						rects[rectIdx].right = command.Rects[rectIdx].MaxX;

						rects[rectIdx].top = command.Rects[rectIdx].MinY;
						rects[rectIdx].bottom = command.Rects[rectIdx].MaxY;
					}

					commandList->RSSetScissorRects(command.Count, rects);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::SetRenderTargets:
				{
					SetRenderTargetsCommand& command = SetRenderTargetsCommand::ReadFrom(commandBufferReader);

					for (uint32_t renderTargetIdx = 0; renderTargetIdx < command.Count; ++renderTargetIdx)
					{
						Texture& target = m_Backend->GetTextureAllocator().Get(uint64_t(command.Targets[renderTargetIdx]));
						// #FIXME shouldn't we request this state transition only when really apply render target in volatile state tracker
						m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_RENDER_TARGET);
						volatileState->SetRenderTarget(renderTargetIdx, &target);
					}

					volatileState->SetUsedRenderTargets(command.Count);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::SetDepthStencil:
				{
					SetDepthStencilCommand& command = SetDepthStencilCommand::ReadFrom(commandBufferReader);

					Texture& target = m_Backend->GetTextureAllocator().Get(uint64_t(command.DepthStencil));
					// #FIXME shouldn't we request this state transition only when really apply render target in volatile state tracker
					m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_DEPTH_WRITE);
					volatileState->SetDepthStencil(&target);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::SetRootVariable:
				{
					SetRootVariableCommand& command = SetRootVariableCommand::ReadFrom(commandBufferReader);

					volatileState->SetRootConstant(command.Index, command.Value);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::UseAsConstantBuffer:
				{
					UseAsConstantBufferCommand& command = UseAsConstantBufferCommand::ReadFrom(commandBufferReader);
					GfxBuffer& target = m_Backend->GetBufferAllocator().Get(uint64_t(command.Buffer));

					m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::UseAsReadableResource:
				{
					UseAsReadableResourceCommand& command = UseAsReadableResourceCommand::ReadFrom(commandBufferReader);

					if (command.Buffer != INVALID_BUFFER_HANDLE)
					{
						GfxBuffer& target = m_Backend->GetBufferAllocator().Get(uint64_t(command.Buffer));

						m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
					}
					else
					{
						hdAssert(command.Texture != INVALID_TEXTURE_HANDLE, u8"UseAsReadableResource command parameters are invalid.");

						Texture& target = m_Backend->GetTextureAllocator().Get(uint64_t(command.Texture));

						m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
					}
				}
				break;


			// -------------------------------------------------------------------------------------------------------------
			case GraphicCommandType::UseAsWriteableResource:
				{
					UseAsWriteableResourceCommand& command = UseAsWriteableResourceCommand::ReadFrom(commandBufferReader);

					if (command.Buffer != INVALID_BUFFER_HANDLE)
					{
						GfxBuffer& target = m_Backend->GetBufferAllocator().Get(uint64_t(command.Buffer));

						m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
					}
					else
					{
						hdAssert(command.Texture != INVALID_TEXTURE_HANDLE, u8"UseAsWriteableResource command parameters are invalid.");

						Texture& target = m_Backend->GetTextureAllocator().Get(uint64_t(command.Texture));

						m_ResourceStateTracker->RequestTransition(target.GetStateTrackedData(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
					}
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
		ScopedScratchMemory scopedScratch{};

		const uint32_t numRootParameters = 1;
		D3D12_ROOT_PARAMETER1* rootParameters = hdAllocate(Scratch(), D3D12_ROOT_PARAMETER1, numRootParameters);

		rootParameters[0] = {};
		rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		rootParameters[0].Constants.Num32BitValues = cfg::NumRootConstants();
		rootParameters[0].Constants.ShaderRegister = 0;
		rootParameters[0].Constants.RegisterSpace = 0;

		const uint32_t numStaticSamplers = 3;
		D3D12_STATIC_SAMPLER_DESC* staticSamplers = hdAllocate(Scratch(), D3D12_STATIC_SAMPLER_DESC, numStaticSamplers);

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

#if defined(HD_ENABLE_RESOURCE_COOKING)
	void DevicePlatform::RegisterRenderStateForRebuild(RenderStatePlatform* renderState)
	{
		m_RenderStatesToRebuild.push_back(renderState);
	}

	void DevicePlatform::UnregisterRenderStateForRebuild(RenderStatePlatform* renderState)
	{
		std::erase(m_RenderStatesToRebuild, renderState);
	}
#endif

	BufferHandle Device::CreateBuffer(uint32_t numElements, uint32_t elementSize, BufferFlags flags)
	{
		size_t bufferSize = size_t(numElements) * elementSize;
		size_t bufferAlignment = flags.IsSet(BufferFlagsBits::ConstantBuffer) ? D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT : elementSize;

		if (flags.IsSet(BufferFlagsBits::ConstantBuffer))
		{
			bufferSize = AlignAbove(bufferSize, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		}

		HeapAllocator::Usage bufferUsage = flags.IsSet(BufferFlagsBits::Transient) ? HeapAllocator::Usage::Transient : HeapAllocator::Usage::Persistent;
		HeapAllocator::Allocation bufferAllocation = m_HeapAllocator->Allocate(bufferSize, bufferAlignment, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS, bufferUsage);

		hdEnsure(bufferAllocation.IsValid(), u8"Cannot allocate GPU memory for resource.");

		GfxBuffer* buffer{};
		BufferHandle result = BufferHandle(m_Backend->GetBufferAllocator().Allocate(&buffer));

		new(buffer)GfxBuffer(*static_cast<Device*>(this), bufferAllocation, numElements, elementSize, flags);

		return result;
	}

	void Device::DestroyBuffer(BufferHandle handle)
	{
		if (m_Backend->GetBufferAllocator().IsValid(uint64_t(handle)))
		{
			m_RecentBuffersToFree.push_back(handle);
		}
	}

	void Device::DestroyBufferImmediate(BufferHandle handle)
	{
		VirtualPoolAllocator<GfxBuffer>& bufferAllocator = m_Backend->GetBufferAllocator();

		if (bufferAllocator.IsValid(uint64_t(handle)))
		{
			GfxBuffer& buffer = bufferAllocator.Get(uint64_t(handle));
			buffer.Free(*this);
			bufferAllocator.Free(uint64_t(handle));
		}
	}

	uint32_t Device::GetCBVShaderIndex(BufferHandle handle)
	{
		VirtualPoolAllocator<GfxBuffer>& bufferAllocator = m_Backend->GetBufferAllocator();

		hdAssert(bufferAllocator.IsValid(uint64_t(handle)), u8"GfxBuffer handle is invalid.");

		GfxBuffer& buffer = bufferAllocator.Get(uint64_t(handle));
		DescriptorSRV descriptor = buffer.GetCBV();

		hdAssert(descriptor, u8"GfxBuffer doesn't have CBV view.");

		return descriptor.HeapIndex;
	}

	uint32_t Device::GetSRVShaderIndex(BufferHandle handle)
	{
		VirtualPoolAllocator<GfxBuffer>& bufferAllocator = m_Backend->GetBufferAllocator();

		hdAssert(bufferAllocator.IsValid(uint64_t(handle)), u8"GfxBuffer handle is invalid.");

		GfxBuffer& buffer = bufferAllocator.Get(uint64_t(handle));
		DescriptorSRV descriptor = buffer.GetSRV();

		hdAssert(descriptor, u8"GfxBuffer doesn't have SRV view.");

		return descriptor.HeapIndex;
	}

	uint32_t Device::GetUAVShaderIndex(BufferHandle handle)
	{
		VirtualPoolAllocator<GfxBuffer>& bufferAllocator = m_Backend->GetBufferAllocator();

		hdAssert(bufferAllocator.IsValid(uint64_t(handle)), u8"GfxBuffer handle is invalid.");

		GfxBuffer& buffer = bufferAllocator.Get(uint64_t(handle));
		DescriptorSRV descriptor = buffer.GetUAV();

		hdAssert(descriptor, u8"GfxBuffer doesn't have UAV view.");

		return descriptor.HeapIndex;
	}

	TextureHandle Device::CreateTexture(uint64_t width, uint32_t height, uint16_t depth, uint16_t mipLevels, GraphicFormat format, TextureFlags flags, TextureDimenstion dimension,
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

		if (flags.IsSet(TextureFlagsBits::RenderTarget))
			textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		if (flags.IsSet(TextureFlagsBits::DepthStencil))
			textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		if (flags.IsSet(TextureFlagsBits::UnorderedAccess))
			textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		if (!flags.IsSet(TextureFlagsBits::ShaderResource))
			textureDesc.Flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

		bool isRenderTarget = flags.IsSet(TextureFlagsBits::RenderTarget) || flags.IsSet(TextureFlagsBits::DepthStencil);

		hdAssert(!(isRenderTarget && isCube), u8"Cubemaps as render targets are not supported.");

		D3D12_CLEAR_VALUE textureClearValue{};
		if (clearValue != nullptr)
		{
			if (flags.IsSet(TextureFlagsBits::RenderTarget))
			{
				textureClearValue.Format = ConvertToWriteableFormat(format);
				textureClearValue.Color[0] = clearValue[0];
				textureClearValue.Color[1] = clearValue[1];
				textureClearValue.Color[2] = clearValue[2];
				textureClearValue.Color[3] = clearValue[3];
			}
			else if (flags.IsSet(TextureFlagsBits::DepthStencil))
			{
				textureClearValue.Format = ConvertToWriteableFormat(format);
				textureClearValue.DepthStencil.Depth = clearValue[0];
				textureClearValue.DepthStencil.Stencil = uint8_t(clearValue[1]);
			}
		}

		D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
		if (flags.IsSet(TextureFlagsBits::RenderTarget))
		{
			initialState = D3D12_RESOURCE_STATE_RENDER_TARGET;
		}
		else if (flags.IsSet(TextureFlagsBits::DepthStencil))
		{
			initialState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}

		D3D12_HEAP_FLAGS heapFlags = isRenderTarget ? D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES : D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;

		D3D12_RESOURCE_ALLOCATION_INFO allocationInfo = m_Device->GetResourceAllocationInfo(0, 1, &textureDesc);

		HeapAllocator::Allocation allocation = m_HeapAllocator->Allocate(allocationInfo.SizeInBytes, allocationInfo.Alignment, D3D12_HEAP_TYPE_DEFAULT, heapFlags, 
		                                                                 HeapAllocator::Usage::Persistent);

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
			m_RecentTexturesToFree.push_back(handle);
		}
	}

	void Device::DestroyTextureImmediate(TextureHandle handle)
	{
		VirtualPoolAllocator<Texture>& textureAllocator = m_Backend->GetTextureAllocator();

		if (textureAllocator.IsValid(uint64_t(handle)))
		{
			Texture& texture = textureAllocator.Get(uint64_t(handle));
			texture.Free(*this);
			textureAllocator.Free(uint64_t(handle));
		}
	}

	uint32_t Device::GetSRVShaderIndex(TextureHandle handle)
	{
		VirtualPoolAllocator<Texture>& textureAllocator = m_Backend->GetTextureAllocator();

		hdAssert(textureAllocator.IsValid(uint64_t(handle)), u8"Texture handle is invalid.");

		Texture& texture = textureAllocator.Get(uint64_t(handle));
		DescriptorSRV descriptor = texture.GetSRV();

		hdAssert(descriptor, u8"Texture doesn't have SRV view.");

		return descriptor.HeapIndex;
	}

	uint32_t Device::GetUAVShaderIndex(TextureHandle handle)
	{
		VirtualPoolAllocator<Texture>& textureAllocator = m_Backend->GetTextureAllocator();

		hdAssert(textureAllocator.IsValid(uint64_t(handle)), u8"Texture handle is invalid.");

		Texture& texture = textureAllocator.Get(uint64_t(handle));
		DescriptorSRV descriptor = texture.GetUAV();

		hdAssert(descriptor, u8"Texture doesn't have SRV view.");

		return descriptor.HeapIndex;
	}

	void Device::GetTextureDimensions(TextureHandle handle, uint64_t& outWidth, uint32_t& outHeight)
	{
		VirtualPoolAllocator<Texture>& textureAllocator = m_Backend->GetTextureAllocator();

		hdAssert(textureAllocator.IsValid(uint64_t(handle)), u8"Texture handle is invalid.");

		Texture& texture = textureAllocator.Get(uint64_t(handle));

		outWidth = texture.GetWidth();
		outHeight = texture.GetHeight();
	}

	void Device::RecycleResources(uint64_t currentMarker, uint64_t completedMarker)
	{
		m_GraphicsCommandListManager->RecycleResources(currentMarker, completedMarker);
		m_ComputeCommandListManager->RecycleResources(currentMarker, completedMarker);
		m_CopyCommandListManager->RecycleResources(currentMarker, completedMarker);

		for (BufferHandle& buffer : m_RecentBuffersToFree)
		{
			m_BuffersToFree.emplace_back(currentMarker, buffer);
		}
		m_RecentBuffersToFree.clear();

		for (TextureHandle& texture : m_RecentTexturesToFree)
		{
			m_TexturesToFree.emplace_back(currentMarker, texture);
		}
		m_RecentTexturesToFree.clear();

		VirtualPoolAllocator<GfxBuffer>& bufferAllocator = m_Backend->GetBufferAllocator();
		for (uint32_t bufferIdx = 0; bufferIdx < m_BuffersToFree.size(); ++bufferIdx)
		{
			ResourceHolder<BufferHandle>& holder = m_BuffersToFree[bufferIdx];

			if (holder.FrameMarker <= completedMarker)
			{
				GfxBuffer& texture = bufferAllocator.Get(uint64_t(holder.Resource));
				texture.Free(*this);
				bufferAllocator.Free(uint64_t(holder.Resource));

				m_BuffersToFree.erase(std::next(m_BuffersToFree.begin(), bufferIdx));
				bufferIdx -= 1;
			}
		}

		VirtualPoolAllocator<Texture>& textureAllocator = m_Backend->GetTextureAllocator();
		for (uint32_t textureIdx = 0; textureIdx < m_TexturesToFree.size(); ++textureIdx)
		{
			ResourceHolder<TextureHandle>& holder = m_TexturesToFree[textureIdx];

			if (holder.FrameMarker <= completedMarker)
			{
				Texture& texture = textureAllocator.Get(uint64_t(holder.Resource));
				texture.Free(*this);
				textureAllocator.Free(uint64_t(holder.Resource));

				m_TexturesToFree.erase(std::next(m_TexturesToFree.begin(), textureIdx));
				textureIdx -= 1;
			}
		}

		m_HeapAllocator->RecycleResources(currentMarker, completedMarker);
	}

	void Device::GetMemoryBudgets(size_t& outLocalBudget, size_t& outLocalUsage, size_t& outNonlocalBudget, size_t& outNonlocalUsage)
	{
		DXGI_QUERY_VIDEO_MEMORY_INFO localInfo{};
		DXGI_QUERY_VIDEO_MEMORY_INFO nonlocalInfo{};

		hdEnsure(m_Adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &localInfo));
		hdEnsure(m_Adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &nonlocalInfo));

		outLocalBudget = localInfo.Budget;
		outLocalUsage = localInfo.CurrentUsage;

		outNonlocalBudget = nonlocalInfo.CurrentUsage;
		outNonlocalUsage = nonlocalInfo.CurrentUsage;
	}

#if defined(HD_ENABLE_RESOURCE_COOKING)
	void Device::RebuildRenderStates(bool ignoreCache)
	{
		m_Backend->GetShaderManager().ResetShaderCache();

		for (size_t renderStateIdx = 0; renderStateIdx < m_RenderStatesToRebuild.size(); ++renderStateIdx)
		{
			m_RenderStatesToRebuild[renderStateIdx]->Rebuild(ignoreCache);
		}
	}
#endif
}

#endif

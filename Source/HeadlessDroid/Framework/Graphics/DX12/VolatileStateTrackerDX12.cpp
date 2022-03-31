#include "Config/Bootstrap.h"

#include "Framework/Graphics/DX12/VolatileStateTrackerDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Debug/Assert.h"
#include "Framework/Graphics/DX12/TextureDX12.h"
#include "Framework/Graphics/DX12/UtilsDX12.h"
#include "Framework/Graphics/RenderState.h"
#include "Framework/Memory/FrameworkMemoryInterface.h"

namespace hd
{
    namespace gfx
    {
        VolatileStateTracker::VolatileStateTracker()
            : m_CurrentGraphicsState{}
            , m_ChangedGraphicsState{}
            , m_CurrentComputeState{}
            , m_ChangedComputeState{}
        {
            memset(m_CurrentGraphicsState.RootConstants, int(UINT32_MAX), cfg::NumRootConstants() * sizeof(uint32_t));
            memset(m_ChangedGraphicsState.RootConstants, int(UINT32_MAX), cfg::NumRootConstants() * sizeof(uint32_t));
            memset(m_CurrentComputeState.RootConstants, int(UINT32_MAX), cfg::NumRootConstants() * sizeof(uint32_t));
            memset(m_ChangedComputeState.RootConstants, int(UINT32_MAX), cfg::NumRootConstants() * sizeof(uint32_t));
        }

        void VolatileStateTracker::SetRootSignature(ID3D12RootSignature* rootSignature)
        {
            m_ChangedGraphicsState.RootSignature = rootSignature;
            m_ChangedComputeState.RootSignature = rootSignature;
        }

        void VolatileStateTracker::SetRenderState(RenderState* renderState)
        {
            m_ChangedGraphicsState.PrimitiveTypeCache = renderState->GetPrimitiveType();

            m_ChangedGraphicsState.PipelineState = renderState->GetNativePipelineState();
            m_ChangedComputeState.PipelineState = renderState->GetNativePipelineState();

            m_ChangedGraphicsState.PrimitiveTopology = ConvertToPrimitiveTopology(m_ChangedGraphicsState.PrimitiveTypeCache, m_ChangedGraphicsState.TopologyTypeCache);
        }

        void VolatileStateTracker::SetTopologyType(TopologyType topologyType)
        {
            m_ChangedGraphicsState.TopologyTypeCache = topologyType;

            m_ChangedGraphicsState.PrimitiveTopology = ConvertToPrimitiveTopology(m_ChangedGraphicsState.PrimitiveTypeCache, m_ChangedGraphicsState.TopologyTypeCache);
        }

        void VolatileStateTracker::SetRenderTarget(uint32_t index, Texture* target)
        {
            m_ChangedGraphicsState.RenderTargets[index] = target;
        }

        void VolatileStateTracker::SetUsedRenderTargets(uint32_t count)
        {
            m_ChangedGraphicsState.UsedRenderTargets = count;
        }

        void VolatileStateTracker::SetDepthStencil(Texture* target)
        {
            m_ChangedGraphicsState.DepthStencil = target;
        }

        void VolatileStateTracker::SetRootConstant(uint32_t index, uint32_t value)
        {
            hdAssert(index < cfg::NumRootConstants(), u8"Invalid root constant index.");

            m_ChangedGraphicsState.RootConstants[index] = value;
            m_ChangedComputeState.RootConstants[index] = value;
        }

        void VolatileStateTracker::ApplyChangedStatesForGrahics(ID3D12GraphicsCommandList& commandList)
        {
            ApplyChangedStatesInternal(commandList, m_CurrentGraphicsState, m_ChangedGraphicsState, true);
        }

        void VolatileStateTracker::ApplyChangedStatesForCompute(ID3D12GraphicsCommandList& commandList)
        {
            ApplyChangedStatesInternal(commandList, m_CurrentComputeState, m_ChangedComputeState, false);
        }

        void VolatileStateTracker::ApplyChangedStatesInternal(ID3D12GraphicsCommandList& commandList, VolatileState& currentState, VolatileState& changedState, bool graphics)
        {
            ScopedScratchMemory scopedScratch{};

            if (currentState.RootSignature != changedState.RootSignature)
            {
                if (graphics)
                {
                    commandList.SetGraphicsRootSignature(changedState.RootSignature);
                }
                else
                {
                    commandList.SetComputeRootSignature(changedState.RootSignature);
                }
            }

            if (currentState.PipelineState != changedState.PipelineState)
            {
                commandList.SetPipelineState(changedState.PipelineState);
            }

            if (currentState.PrimitiveTopology != changedState.PrimitiveTopology)
            {
                commandList.IASetPrimitiveTopology(changedState.PrimitiveTopology);
            }

            {
                bool renderTargetsDirty = currentState.UsedRenderTargets != changedState.UsedRenderTargets;
                renderTargetsDirty |= currentState.DepthStencil != changedState.DepthStencil;
                for (uint32_t renderTargetIdx = 0; renderTargetIdx < changedState.UsedRenderTargets; ++renderTargetIdx)
                {
                    renderTargetsDirty |= currentState.RenderTargets[renderTargetIdx] != changedState.RenderTargets[renderTargetIdx];
                }

                if (renderTargetsDirty)
                {
                    D3D12_CPU_DESCRIPTOR_HANDLE* rtvDescriptors = hdAllocate(mem::Scratch(), D3D12_CPU_DESCRIPTOR_HANDLE, changedState.UsedRenderTargets);
                    for (uint32_t renderTargetIdx = 0; renderTargetIdx < changedState.UsedRenderTargets; ++renderTargetIdx)
                    {
                        rtvDescriptors[renderTargetIdx] = changedState.RenderTargets[renderTargetIdx]->GetRTV().HandleCPU;
                    }

                    D3D12_CPU_DESCRIPTOR_HANDLE dsvDescriptor{};
                    if (changedState.DepthStencil != nullptr)
                    {
                        dsvDescriptor = changedState.DepthStencil->GetDSV().HandleCPU;
                    }

                    commandList.OMSetRenderTargets(changedState.UsedRenderTargets, rtvDescriptors, false, changedState.DepthStencil != nullptr ? &dsvDescriptor : nullptr);
                }
            }

            {
                uint32_t dirtyRangeStartIdx = UINT32_MAX;
                for (uint32_t rootConstantIdx = 0; rootConstantIdx < cfg::NumRootConstants(); ++rootConstantIdx)
                {
                    if (currentState.RootConstants[rootConstantIdx] != changedState.RootConstants[rootConstantIdx])
                    {
                        // Update dirty range start with the new index if it is not updated
                        if (dirtyRangeStartIdx == UINT32_MAX)
                        {
                            dirtyRangeStartIdx = rootConstantIdx;
                        }
                    }
                    else
                    {
                        // Update the range and reset dirty range start
                        if (dirtyRangeStartIdx != UINT32_MAX)
                        {
                            UpdateRootConstants(commandList, changedState, dirtyRangeStartIdx, rootConstantIdx, graphics);
                            dirtyRangeStartIdx = UINT32_MAX;
                        }
                    }
                }

                // Check if something is not updated
                if (dirtyRangeStartIdx != UINT32_MAX)
                {
                    UpdateRootConstants(commandList, changedState, dirtyRangeStartIdx, cfg::NumRootConstants(), graphics);
                }
            }

            currentState = changedState;
        }

        void VolatileStateTracker::UpdateRootConstants(ID3D12GraphicsCommandList& commandList, VolatileState& state, uint32_t beginIdx, uint32_t endIdx, bool graphics)
        {
            uint32_t numConstantsToUpdate = endIdx - beginIdx;
            if (numConstantsToUpdate == 1)
            {
                if (graphics)
                {
                    commandList.SetGraphicsRoot32BitConstant(0, state.RootConstants[beginIdx], beginIdx);
                }
                else
                {
                    commandList.SetComputeRoot32BitConstant(0, state.RootConstants[beginIdx], beginIdx);
                }
            }
            else
            {
                if (graphics)
                {
                    commandList.SetGraphicsRoot32BitConstants(0, numConstantsToUpdate, &state.RootConstants[beginIdx], beginIdx);
                }
                else
                {
                    commandList.SetComputeRoot32BitConstants(0, numConstantsToUpdate, &state.RootConstants[beginIdx], beginIdx);
                }
            }
        }
    }
}

#endif
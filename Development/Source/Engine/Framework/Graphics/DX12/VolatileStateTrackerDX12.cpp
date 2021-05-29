#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/VolatileStateTrackerDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/DX12/TextureDX12.h"
#include "Engine/Framework/Graphics/DX12/UtilsDX12.h"
#include "Engine/Framework/Graphics/RenderState.h"
#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/Memory/FrameworkMemoryInterface.h"

namespace hd
{
    namespace gfx
    {
        VolatileStateTracker::VolatileStateTracker()
            : m_CurrentState{}
            , m_ChangedState{}
        {

        }

        void VolatileStateTracker::SetRenderState(RenderState* renderState)
        {
            m_ChangedState.PrimitiveTypeCache = renderState->GetPrimitiveType();

            m_ChangedState.PipelineState = renderState->GetNativePipelineState();
            m_ChangedState.PrimitiveTopology = ConvertToPrimitiveTopology(m_ChangedState.PrimitiveTypeCache, m_ChangedState.TopologyTypeCache);
        }

        void VolatileStateTracker::SetTopologyType(TopologyType topologyType)
        {
            m_ChangedState.TopologyTypeCache = topologyType;

            m_ChangedState.PrimitiveTopology = ConvertToPrimitiveTopology(m_ChangedState.PrimitiveTypeCache, m_ChangedState.TopologyTypeCache);
        }

        void VolatileStateTracker::SetRenderTarget(uint32_t index, Texture* target)
        {
            m_ChangedState.RenderTargets[index] = target;
        }

        void VolatileStateTracker::SetUsedRenderTargets(uint32_t count)
        {
            m_ChangedState.UsedRenderTargets = count;
        }

        void VolatileStateTracker::SetDepthStencil(Texture* target)
        {
            m_ChangedState.DepthStencil = target;
        }

        void VolatileStateTracker::ApplyChangedStates(ID3D12GraphicsCommandList& commandList)
        {
            mem::AllocationScope scratchScope{ mem::GetScratchAllocator() };

            if (m_CurrentState.PipelineState != m_ChangedState.PipelineState)
            {
                commandList.SetPipelineState(m_ChangedState.PipelineState);
            }

            if (m_CurrentState.PrimitiveTopology != m_ChangedState.PrimitiveTopology)
            {
                commandList.IASetPrimitiveTopology(m_ChangedState.PrimitiveTopology);
            }

            {
                bool renderTargetsDirty = m_CurrentState.UsedRenderTargets != m_ChangedState.UsedRenderTargets;
                renderTargetsDirty |= m_CurrentState.DepthStencil != m_ChangedState.DepthStencil;
                for (uint32_t renderTargetIdx = 0; renderTargetIdx < m_ChangedState.UsedRenderTargets; ++renderTargetIdx)
                {
                    renderTargetsDirty |= m_CurrentState.RenderTargets[renderTargetIdx] != m_ChangedState.RenderTargets[renderTargetIdx];
                }

                if (renderTargetsDirty)
                {
                    D3D12_CPU_DESCRIPTOR_HANDLE* rtvDescriptors = scratchScope.AllocatePODArray<D3D12_CPU_DESCRIPTOR_HANDLE>(m_ChangedState.UsedRenderTargets);
                    for (uint32_t renderTargetIdx = 0; renderTargetIdx < m_ChangedState.UsedRenderTargets; ++renderTargetIdx)
                    {
                        rtvDescriptors[renderTargetIdx] = m_ChangedState.RenderTargets[renderTargetIdx]->GetRTV().HandleCPU;
                    }
                    
                    D3D12_CPU_DESCRIPTOR_HANDLE dsvDescriptor{};
                    if (m_ChangedState.DepthStencil != nullptr)
                    {
                        dsvDescriptor = m_ChangedState.DepthStencil->GetDSV().HandleCPU;
                    }

                    commandList.OMSetRenderTargets(m_ChangedState.UsedRenderTargets, rtvDescriptors, false, m_ChangedState.DepthStencil != nullptr ? &dsvDescriptor : nullptr);
                }
            }

            m_CurrentState = m_ChangedState;
        }
    }
}

#endif
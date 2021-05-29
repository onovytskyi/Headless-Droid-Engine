#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace gfx
    {
        class RenderState;
        class Texture;

        class VolatileStateTracker
        {
        public:
            struct VolatileState
            {
                ID3D12PipelineState* PipelineState;
                D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology;

                Texture* RenderTargets[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
                uint32_t UsedRenderTargets;
                Texture* DepthStencil;

                PrimitiveType PrimitiveTypeCache;
                TopologyType TopologyTypeCache;
            };

            VolatileStateTracker();

            void SetRenderState(RenderState* renderState);
            void SetTopologyType(TopologyType topologyType);
            void SetRenderTarget(uint32_t index, Texture* target);
            void SetUsedRenderTargets(uint32_t count);
            void SetDepthStencil(Texture* target);

            void ApplyChangedStates(ID3D12GraphicsCommandList& commandList);

        private:
            VolatileState m_CurrentState;
            VolatileState m_ChangedState;
        };
    }
}

#endif
#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Graphics/GraphicsTypes.h"

namespace hd
{
	class RenderState;
	class Texture;

	class VolatileStateTracker
	{
	public:
		struct VolatileState
		{
			ID3D12RootSignature* RootSignature;
			ID3D12PipelineState* PipelineState;
			D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology;

			Texture* RenderTargets[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
			uint32_t UsedRenderTargets;
			Texture* DepthStencil;

			PrimitiveType PrimitiveTypeCache;
			TopologyType TopologyTypeCache;
			uint32_t RootConstants[cfg::NumRootConstants()];
		};

		VolatileStateTracker();

		void SetRootSignature(ID3D12RootSignature* rootSignature);
		void SetRenderState(RenderState* renderState);
		void SetTopologyType(TopologyType topologyType);
		void SetRenderTarget(uint32_t index, Texture* target);
		void SetUsedRenderTargets(uint32_t count);
		void SetDepthStencil(Texture* target);
		void SetRootConstant(uint32_t index, uint32_t value);

		void ApplyChangedStatesForGrahics(ID3D12GraphicsCommandList& commandList);
		void ApplyChangedStatesForCompute(ID3D12GraphicsCommandList& commandList);

	private:
		void ApplyChangedStatesInternal(ID3D12GraphicsCommandList& commandList, VolatileState& currentState, VolatileState& changedState, bool graphics);
		void UpdateRootConstants(ID3D12GraphicsCommandList& commandList, VolatileState& state, uint32_t beginIdx, uint32_t endIdx, bool graphics);

		VolatileState m_CurrentGraphicsState;
		VolatileState m_CurrentComputeState;
		VolatileState m_ChangedGraphicsState;
		VolatileState m_ChangedComputeState;
	};
}

#endif

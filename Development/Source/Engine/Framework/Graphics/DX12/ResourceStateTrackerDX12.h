#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Utils/BufferArray.h"

namespace hd
{
    namespace mem
    {
        class AllocationScope;
    }

    namespace gfx
    {
        class ResourceStateTracker
        {
        public:
            struct StateTrackedData
            {
                static const size_t MAX_SUBRESOURCES = 32;

                ID3D12Resource* Resource;
                D3D12_RESOURCE_STATES State;

                D3D12_RESOURCE_STATES SubresourceStates[MAX_SUBRESOURCES];
                uint32_t UsedSubresources;
                bool SubresourceStatesDirty;
            };

            ResourceStateTracker(mem::AllocationScope& allocationScope);

            void RequestTransition(StateTrackedData& data, D3D12_RESOURCE_STATES toState);
            void RequestSubresourceTransition(StateTrackedData& data, uint32_t subresource, D3D12_RESOURCE_STATES toState);

            void ApplyTransitions(ID3D12GraphicsCommandList& commandList);

        private:
            struct StateTransitionRequest
            {
                StateTrackedData* Data;
                D3D12_RESOURCE_STATES TargetState;
                uint32_t Subresource;
            };

            util::BufferArray<StateTransitionRequest> m_TransitionRequests;
        };
    }
}

#endif
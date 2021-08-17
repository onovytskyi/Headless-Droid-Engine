#pragma once

#if defined(HD_GRAPHICS_API_DX12)

namespace hd
{
    class Allocator;

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

            ResourceStateTracker(Allocator& generalAllocator);

            void RequestTransition(StateTrackedData& data, D3D12_RESOURCE_STATES toState);
            void RequestSubresourceTransition(StateTrackedData& data, uint32_t subresource, D3D12_RESOURCE_STATES toState);

            void ApplyTransitions(ID3D12GraphicsCommandList& commandList);

        private:
            Allocator& m_GeneralAllocator;

            struct StateTransitionRequest
            {
                StateTrackedData* Data;
                D3D12_RESOURCE_STATES TargetState;
                uint32_t Subresource;
            };

            std::pmr::vector<StateTransitionRequest> m_TransitionRequests;
        };
    }
}

#endif
#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/DX12/ResourceStateTrackerDX12.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Framework/Graphics/GraphicsTypes.h"
#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/Memory/FrameworkMemoryInterface.h"

namespace hd
{
    namespace gfx
    {
        ResourceStateTracker::ResourceStateTracker(mem::AllocationScope& allocationScope)
            : m_TransitionRequests{ allocationScope, cfg::MaxTransitionRequests() }
        {

        }

        void ResourceStateTracker::RequestTransition(StateTrackedData& data, D3D12_RESOURCE_STATES toState)
        {
            RequestSubresourceTransition(data, ALL_SUBRESOURCES, toState);
        }

        void ResourceStateTracker::RequestSubresourceTransition(StateTrackedData& data, uint32_t subresource, D3D12_RESOURCE_STATES toState)
        {
            m_TransitionRequests.Add({ &data, toState, subresource });
        }

        bool IsReadOnlyState(D3D12_RESOURCE_STATES state)
        {
            if (state == D3D12_RESOURCE_STATE_COMMON)
                return false;

            constexpr D3D12_RESOURCE_STATES writeBits = D3D12_RESOURCE_STATE_RENDER_TARGET | D3D12_RESOURCE_STATE_UNORDERED_ACCESS | D3D12_RESOURCE_STATE_DEPTH_WRITE |
                D3D12_RESOURCE_STATE_STREAM_OUT | D3D12_RESOURCE_STATE_COPY_DEST | D3D12_RESOURCE_STATE_RESOLVE_DEST | D3D12_RESOURCE_STATE_PREDICATION | D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE |
                D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE | D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE;
            if (state & writeBits)
                return false;

            return true;
        }

        void WriteTransitionForSingleSubresource(ID3D12Resource* resource, D3D12_RESOURCE_STATES& subresourceState, D3D12_RESOURCE_STATES targetState,
            D3D12_RESOURCE_BARRIER* barriers, uint32_t& barriersCount, uint32_t subresourceIdx, bool mergeTransitions)
        {
            D3D12_RESOURCE_STATES nextState = subresourceState;
            if (mergeTransitions && IsReadOnlyState(targetState) && IsReadOnlyState(nextState))
            {
                nextState |= targetState;
            }
            else
            {
                nextState = targetState;
            }

            if (subresourceState != nextState)
            {
                D3D12_RESOURCE_BARRIER& transitionBarrier = barriers[barriersCount];
                transitionBarrier = {};
                transitionBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                transitionBarrier.Transition.pResource = resource;
                transitionBarrier.Transition.StateBefore = subresourceState;
                transitionBarrier.Transition.StateAfter = nextState;
                transitionBarrier.Transition.Subresource = subresourceIdx;

                subresourceState = nextState;

                barriersCount += 1;
            }
        }

        void ResourceStateTracker::ApplyTransitions(ID3D12GraphicsCommandList& commandList)
        {
            mem::AllocationScope scratchScope(mem::GetScratchAllocator());

            size_t transitionsAllocationSize = sizeof(D3D12_RESOURCE_BARRIER) * m_TransitionRequests.GetSize() * StateTrackedData::MAX_SUBRESOURCES;
            D3D12_RESOURCE_BARRIER* transitionBarriers = reinterpret_cast<D3D12_RESOURCE_BARRIER*>(scratchScope.AllocateMemory(transitionsAllocationSize, alignof(D3D12_RESOURCE_BARRIER)));

            uint32_t barriersCount = 0;
            for (uint32_t requestIdx = 0; requestIdx < m_TransitionRequests.GetSize(); ++requestIdx)
            {
                StateTransitionRequest& request = m_TransitionRequests[requestIdx];
                ID3D12Resource* nativeResource = request.Data->Resource;
                hdAssert(nativeResource != nullptr);

                if (request.Subresource == ALL_SUBRESOURCES)
                {
                    if (!request.Data->SubresourceStatesDirty)
                    {
                        WriteTransitionForSingleSubresource(nativeResource, request.Data->State, request.TargetState,
                            transitionBarriers, barriersCount, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, true);
                    }
                    else
                    {
                        // Force transition all subresources to target state without state merging
                        hdAssert(request.Data->UsedSubresources <= StateTrackedData::MAX_SUBRESOURCES, u8"Used subresources is bigger than max subresources to track.");
                        for (uint32_t subresourceIdx = 0; subresourceIdx < request.Data->UsedSubresources; ++subresourceIdx)
                        {
                            WriteTransitionForSingleSubresource(nativeResource, request.Data->SubresourceStates[subresourceIdx], request.TargetState,
                                transitionBarriers, barriersCount, subresourceIdx, false);
                        }

                        request.Data->State = request.TargetState;
                        request.Data->SubresourceStatesDirty = false;
                    }

                }
                else
                {
                    hdAssert(request.Data->UsedSubresources <= StateTrackedData::MAX_SUBRESOURCES, u8"Used subresources is bigger than max subresources to track.");
                    hdAssert(request.Subresource < request.Data->UsedSubresources, u8"Subresource index to transition is out of bounds.");

                    if (!request.Data->SubresourceStatesDirty)
                    {
                        // Initialize all subresource states with the state from resource
                        for (uint32_t subresourceIdx = 0; subresourceIdx < request.Data->UsedSubresources; ++subresourceIdx)
                        {
                            request.Data->SubresourceStates[subresourceIdx] = request.Data->State;
                        }

                        request.Data->SubresourceStatesDirty = true;
                    }

                    WriteTransitionForSingleSubresource(nativeResource, request.Data->SubresourceStates[request.Subresource], request.TargetState,
                        transitionBarriers, barriersCount, request.Subresource, true);
                }
            }

            if (barriersCount > 0)
            {
                commandList.ResourceBarrier(barriersCount, transitionBarriers);
            }

            m_TransitionRequests.Clear();
        }
    }
}

#endif
#pragma once

namespace hd
{
    namespace cfg
    {
        inline constexpr char8_t const* LogFilepath() { return u8"headless_droid_log.txt"; }

        inline constexpr uint32_t MaxFrameLatency() { return 3; }

#if defined(HD_GRAPHICS_API_DX12)
        inline constexpr size_t MaxTransitionRequests() { return 128; }
        inline constexpr uint32_t MaxDescriptorsRTV() { return 1000000; }
        inline constexpr uint32_t MaxDescriptorsDSV() { return 1000000; }
        inline constexpr uint32_t MaxDescriptorsSRV() { return 1000000; }
        inline constexpr uint32_t MaxDescriptorsSampler() { return 2048; }
        inline constexpr uint32_t MaxGraphicsCommandLists() { return 16; }
        inline constexpr uint32_t MaxGraphicsCommandAllocators() { return 24; }
        inline constexpr uint32_t MaxComputeCommandLists() { return 16; }
        inline constexpr uint32_t MaxComputeCommandAllocators() { return 24; }
        inline constexpr uint32_t MaxCopyCommandLists() { return 16; }
        inline constexpr uint32_t MaxCopyCommandAllocators() { return 24; }
        inline constexpr uint32_t MaxBuffersToFreePerFrame() { return 32; }
        inline constexpr uint32_t MaxBuffersToFreeInQueue() { return 64; }
        inline constexpr uint32_t MaxTexturesToFreePerFrame() { return 32; }
        inline constexpr uint32_t MaxTexturesToFreeInQueue() { return 64; }
#endif
    }
}
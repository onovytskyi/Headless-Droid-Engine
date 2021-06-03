#pragma once

namespace hd
{
    namespace cfg
    {
        inline constexpr char8_t const* LogFilepath() { return u8"headless_droid_log.txt"; }

        inline constexpr uint32_t MaxFrameLatency() { return 3; }

        inline constexpr char8_t const* ShadersPath() { return u8"../Development/Source/Shaders/"; }
        inline constexpr char8_t const* MediaPath() { return u8"../Media/"; }

#if defined(HD_BUILD_DEBUG)
        inline constexpr char8_t const* CookedFilePath() { return u8"./Cooked/Debug/"; }
        inline constexpr char8_t const* SymbolsFilePath() { return u8"./Cooked/Symbols/Debug/"; }
#elif defined(HD_BUILD_DEVELOPMENT)
        inline constexpr char8_t const* CookedFilePath() { return u8"./Cooked/Development/"; }
        inline constexpr char8_t const* SymbolsFilePath() { return u8"./Cooked/Symbols/Development/"; }
#else
        inline constexpr char8_t const* CookedFilePath() { return u8"./Cooked/"; }
        inline constexpr char8_t const* SymbolsFilePath() { return u8"./Cooked/Symbols/Release/"; }
#endif

#if defined(HD_GRAPHICS_API_DX12)
        inline constexpr uint32_t NumRootConstants() { return 32; } // See also Common.hlsl
        inline constexpr size_t MaxTransitionRequests() { return 128; }
        inline constexpr uint32_t MaxDescriptorsRTV() { return 1000000; }
        inline constexpr uint32_t MaxDescriptorsDSV() { return 1000000; }
        inline constexpr uint32_t MaxDescriptorsSRV() { return 10000; }
        inline constexpr uint32_t MaxDescriptorsSampler() { return 2048; }
        inline constexpr uint32_t MaxGraphicsCommandLists() { return 16; }
        inline constexpr uint32_t MaxGraphicsCommandAllocators() { return 24; }
        inline constexpr uint32_t MaxComputeCommandLists() { return 16; }
        inline constexpr uint32_t MaxComputeCommandAllocators() { return 24; }
        inline constexpr uint32_t MaxCopyCommandLists() { return 16; }
        inline constexpr uint32_t MaxCopyCommandAllocators() { return 24; }
        inline constexpr uint32_t MaxBuffersToFreePerFrame() { return 32; }
        inline constexpr uint32_t MaxBuffersToFreeInQueue() { return 64; }
        inline constexpr uint32_t MaxTexturesToFreePerFrame() { return 128; }
        inline constexpr uint32_t MaxTexturesToFreeInQueue() { return 256; }
        inline constexpr size_t GPUHeapSize() { return 64 * 1024 * 1024; }
        inline constexpr uint32_t MaxGPUHeaps() { return 128; }
        inline constexpr uint32_t KeepHeapsAliveForFrames() { return 10; }
        inline constexpr char8_t const* GetVSProfile() { return u8"vs_6_6"; }
        inline constexpr char8_t const* GetPSProfile() { return u8"ps_6_6"; }
        inline constexpr char8_t const* GetCSProfile() { return u8"cs_6_6"; }
#endif
    }
}
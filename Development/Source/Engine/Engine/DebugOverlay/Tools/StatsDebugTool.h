#pragma once

#include "Engine/Engine/DebugOverlay/DebugOverlay.h"

namespace hd
{
    namespace gfx
    {
        class Device;
        class Swapchain;
    }

    namespace sys
    {
        class Timer;
    }

    namespace ui
    {
        class StatsDebugTool : public DebugOverlay::Tool
        {
        public:
            StatsDebugTool();

            bool& GetVisibleRef() override;
            bool IsAlwaysVisible() const override;
            const char* GetMenuName() const override;
            const char* GetToolName() const override;
            void Draw() override;

            void Update(sys::Timer& timer, gfx::Swapchain& swapchain, gfx::Device& device);

        private:
            bool m_Visible;

            float m_FPS;
            float m_CPU;
            float m_GPU;
            uint64_t m_CPUFrameId;
            uint64_t m_GPUFrameId;
            size_t m_LocalBudget;
            size_t m_LocalUsage;
            size_t m_NonlocalBudget;
            size_t m_NonlocalUsage;
        };
    }
}
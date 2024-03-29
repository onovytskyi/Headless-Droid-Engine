#pragma once

#include "Engine/DebugOverlay/DebugOverlay.h"

namespace hd
{
	class Device;
	class Swapchain;
	class Timer;

    class StatsDebugTool : public DebugOverlay::Tool
    {
    public:
        StatsDebugTool();

        bool& GetVisibleRef() override;
        ToolType GetType() const override;
        const char* GetMenuName() const override;
        const char* GetToolName() const override;
        void ProcessShortcuts() override;
        void Draw() override;

        void Update(Timer& timer, Swapchain& swapchain, Device& device);

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
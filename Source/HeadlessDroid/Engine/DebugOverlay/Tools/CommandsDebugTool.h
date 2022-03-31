#pragma once

#include "Engine/DebugOverlay/DebugOverlay.h"

namespace hd
{
	class Device;

    class CommandsDebugTool : public DebugOverlay::Tool
    {
    public:
        CommandsDebugTool(Device& device);

        bool& GetVisibleRef() override;
        ToolType GetType() const override;
        const char* GetMenuName() const override;
        const char* GetToolName() const override;
        void ProcessShortcuts() override;
        void Draw() override;

    private:
        void ReloadShaders(bool ignoreCache);

        Device* m_Device;
        bool m_Visible;
    };
}
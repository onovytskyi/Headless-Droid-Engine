#pragma once

#include "Engine/Engine/DebugOverlay/DebugOverlay.h"

namespace hd
{
    namespace gfx
    {
        class Device;
    }

    namespace ui
    {
        class CommandsDebugTool : public DebugOverlay::Tool
        {
        public:
            CommandsDebugTool(gfx::Device& device);

            bool& GetVisibleRef() override;
            ToolType GetType() const override;
            const char* GetMenuName() const override;
            const char* GetToolName() const override;
            void ProcessShortcuts() override;
            void Draw() override;

        private:
            void ReloadShaders(bool ignoreCache);

            gfx::Device* m_Device;
            bool m_Visible;
        };
    }
}
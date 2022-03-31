#include "Engine/Config/Bootstrap.h"

#include "Engine/Debug/Log.h"
#include "Engine/Engine/DebugOverlay/Tools/CommandsDebugTool.h"
#include "Engine/Framework/Graphics/Device.h"

namespace hd
{
    namespace ui
    {
        CommandsDebugTool::CommandsDebugTool(gfx::Device& device)
            : m_Device{ &device }
            , m_Visible{ true }
        {

        }

        bool& CommandsDebugTool::GetVisibleRef()
        {
            return m_Visible;
        }

        DebugOverlay::Tool::ToolType CommandsDebugTool::GetType() const
        {
            return ToolType::Menu;
        }

        const char* CommandsDebugTool::GetMenuName() const
        {
            return "Tools";
        }

        const char* CommandsDebugTool::GetToolName() const
        {
            return "Debug Commands";
        }

        void CommandsDebugTool::ProcessShortcuts()
        {
            ImGuiIO& imguiIO = ImGui::GetIO();

            if (imguiIO.KeyCtrl && ImGui::IsKeyReleased('R'))
            {
                ReloadShaders(false);
            }

            if (imguiIO.KeyCtrl && imguiIO.KeyShift && ImGui::IsKeyReleased('R'))
            {
                ReloadShaders(true);
            }
        }

        void CommandsDebugTool::Draw()
        {
            if (ImGui::MenuItem("Reload CHANGED shaders", "Ctrl+R", nullptr, true))
            {
                ReloadShaders(false);
            }

            if (ImGui::MenuItem("Reload ALL shaders", "Ctrl+Shift+R", nullptr, true))
            {
                ReloadShaders(true);
            }
        }

        void CommandsDebugTool::ReloadShaders(bool ignoreCache)
        {
            m_Device->RebuildRenderStates(ignoreCache);
            hdLogInfo(u8"% shaders reloaded", ignoreCache ? u8"All" : u8"Changed");
        }
    }
}
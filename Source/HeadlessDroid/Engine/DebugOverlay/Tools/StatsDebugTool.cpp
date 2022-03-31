#include "Config/Bootstrap.h"

#include "Engine/DebugOverlay/Tools/StatsDebugTool.h"

#include "Foundation/Memory/Utils.h"
#include "Framework/Graphics/Device.h"
#include "Framework/Graphics/Swapchain.h"
#include "Framework/System/Timer.h"

namespace hd
{
    namespace ui
    {
        void GlueLine()
        {
            ImGui::SameLine(0, 0);
        }

        void PutSeparator()
        {
            GlueLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "|");
            GlueLine();
        }

        void PutLeadingSpaces(int places, float value)
        {
            const char* spaces10 = "          ";
            int spacesToPrint = 0;
            int testPlace = places - 1;
            while (testPlace > 1)
            {
                float testValue = std::powf(10.0f, float(testPlace));
                if (testValue < value)
                    break;

                spacesToPrint += 1;
                testPlace -= 1;
            }

            GlueLine();
            ImGui::TextUnformatted(spaces10, spaces10 + std::min(10, spacesToPrint));
            GlueLine();
        }

        ImVec4 ChooseColorForFPS(float fps)
        {
            if (fps >= 60.0f)
            {
                return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            }
            if (fps >= 30.0f)
            {
                return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
            }
            else
            {
                return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }

        ImVec4 ChooseColorForMS(float ms)
        {
            return ChooseColorForFPS(1000.0f / (ms + 0.00001f));
        }

        ImVec4 ChooseColorForBytes(size_t used, size_t max)
        {
            float ratio = float(mem::ToMB(used)) / float(mem::ToMB(max));
            if (ratio <= 0.5f)
            {
                return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            }
            if (ratio <= 0.8f)
            {
                return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
            }
            else
            {
                return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            }
        }

        StatsDebugTool::StatsDebugTool()
            : m_Visible{ true }
            , m_FPS{}
            , m_CPU{}
            , m_GPU{}
            , m_CPUFrameId{}
            , m_GPUFrameId{}
            , m_LocalBudget{}
            , m_LocalUsage{}
            , m_NonlocalBudget{}
            , m_NonlocalUsage{}
        {

        }

        bool& StatsDebugTool::GetVisibleRef()
        {
            return m_Visible;
        }

        DebugOverlay::Tool::ToolType StatsDebugTool::GetType() const
        {
            return ToolType::AlwaysVisible;
        }

        const char* StatsDebugTool::GetMenuName() const
        {
            return "Stats";
        }

        const char* StatsDebugTool::GetToolName() const
        {
            return "Stats Debug Information";
        }

        void StatsDebugTool::ProcessShortcuts()
        {

        }

        void StatsDebugTool::Draw()
        {
            ImGuiContext& context = *ImGui::GetCurrentContext();
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(64, 64, 64, 128));

            float panelHeight = context.FontBaseSize + context.Style.FramePadding.y;
            ImGui::SetNextWindowPos(ImVec2(0.0f, context.IO.DisplaySize.y - panelHeight));
            ImGui::SetNextWindowSize(ImVec2(context.IO.DisplaySize.x, panelHeight));
            bool isOpen = ImGui::Begin("Debug String Visualizer", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);

            ImGui::PopStyleColor();
            ImGui::PopStyleVar(4);

            if (isOpen)
            {
                PutLeadingSpaces(4, m_FPS);
                ImGui::TextColored(ChooseColorForFPS(m_FPS), "%.2f FPS", m_FPS);
                PutSeparator();

                ImGui::TextColored(ChooseColorForMS(m_CPU), "CPU: ");
                GlueLine();
                PutLeadingSpaces(3, m_CPU);
                ImGui::TextColored(ChooseColorForMS(m_CPU), "%.2f ms", m_CPU);
                PutSeparator();

                ImGui::TextColored(ChooseColorForMS(m_GPU), "GPU: ");
                GlueLine();
                PutLeadingSpaces(3, m_GPU);
                ImGui::TextColored(ChooseColorForMS(m_GPU), "%.2f ms", m_GPU);
                PutSeparator();

                ImGui::TextColored(ChooseColorForBytes(m_LocalUsage, m_LocalBudget), "VRAM %lluMb/%lluMb", mem::ToMB(m_LocalUsage), mem::ToMB(m_LocalBudget));
                PutSeparator();

                ImGui::TextColored(ChooseColorForBytes(m_LocalUsage, m_LocalBudget), "SysRAM %lluMb/%lluMb", mem::ToMB(m_NonlocalUsage), mem::ToMB(m_NonlocalBudget));
                PutSeparator();

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "CPU Frame %llu", m_CPUFrameId);
                PutSeparator();

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "GPU Frame %llu", m_GPUFrameId);
                ImGui::End();
            }
        }

        void StatsDebugTool::Update(sys::Timer& timer, gfx::Swapchain& swapchain, gfx::Device& device)
        {
            m_FPS = 1.0f / timer.GetDeltaSeconds();
            m_CPU = timer.GetDeltaSeconds() * 1000.0f;
            m_GPU = 0.0f;
            m_CPUFrameId = swapchain.GetCPUFrame();
            m_GPUFrameId = swapchain.GetGPUFrame();

            device.GetMemoryBudgets(m_LocalBudget, m_LocalUsage, m_NonlocalBudget, m_NonlocalUsage);
        }
    }
}
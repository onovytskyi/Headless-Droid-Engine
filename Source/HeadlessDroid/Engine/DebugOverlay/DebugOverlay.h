#pragma once

#include "Framework/Graphics/GraphicsTypes.h"
#include "Framework/Math/Math.h"
#include "Framework/Utils/CommandBuffer.h"

#include "Externals/imgui/imgui.h"
#include "Externals/imgui/imgui_internal.h"

namespace hd
{
    class Allocator;
    class Backend;
    class Device;
    class RenderState;

    class DebugOverlay
    {
    public:
        class Tool
        {
        public:
            enum class ToolType
            {
                AlwaysVisible,
                Gadget,
                Menu
            };

            virtual ~Tool() {}

            virtual bool& GetVisibleRef() = 0;
            virtual ToolType GetType() const = 0;
            virtual const char* GetMenuName() const = 0;
            virtual const char* GetToolName() const = 0;
            virtual void ProcessShortcuts() = 0;
            virtual void Draw() = 0;
        };

        DebugOverlay(Backend& backend, Device& device, uint32_t width, uint32_t height, GraphicFormat targetFormat);
        ~DebugOverlay();

        void UploadFont(hd::CommandBuffer& commandBuffer);

        void RegisterDebugTool(Tool& tool);

        void Resize(uint32_t width, uint32_t height);

        void OnMouseButon(float mouseX, float mouseY, uint32_t mouseButtonId, bool pressed);
        void OnMouseWheel(float mouseX, float mouseY, float wheel);
        void OnMouseMove(float mouseX, float mouseY);
        void OnKeyboardKey(uint8_t keyID, bool pressed);

        void Render(TextureHandle target, CommandBuffer& graphicCommands);

    private:
        void ConstructFrame();

        Device* m_Device;

        TextureHandle m_DefaultFont;
        Matrix4x4 m_GuiProjectionMatrix;

        RenderState* m_GuiDrawRenderState;

        bool m_Visible;

        std::pmr::vector<Tool*> m_Tools;
    };
}
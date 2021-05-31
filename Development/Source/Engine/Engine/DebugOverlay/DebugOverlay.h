#pragma once

#include "Engine/Framework/Graphics/GraphicsTypes.h"
#include "Engine/Framework/Math/Math.h"
#include "Engine/Framework/Utils/BufferArray.h"
#include "Engine/Framework/Utils/CommandBuffer.h"

#include "Externals/imgui/imgui.h"
#include "Externals/imgui/imgui_internal.h"

namespace hd
{
    namespace gfx
    {
        class Backend;
        class Device;
        class RenderState;
    }

    namespace mem
    {
        class AllocationScope;
    }

    namespace ui
    {
        class DebugOverlay
        {
        public:
            class Tool
            {
            public:
                virtual ~Tool() {}

                virtual bool& GetVisibleRef() = 0;
                virtual bool IsAlwaysVisible() const = 0;
                virtual const char* GetMenuName() const = 0;
                virtual const char* GetToolName() const = 0;
                virtual void Draw() = 0;
            };

            DebugOverlay(gfx::Backend& backend, gfx::Device& device, uint32_t width, uint32_t height, gfx::GraphicFormat targetFormat, mem::AllocationScope& allocationScope, size_t maxTools);
            ~DebugOverlay();

            void UploadFont(hd::util::CommandBuffer& commandBuffer);

            void RegisterDebugTool(Tool& tool);

            void Resize(uint32_t width, uint32_t height);

            void OnMouseButon(float mouseX, float mouseY, uint32_t mouseButtonId, bool pressed);
            void OnMouseWheel(float mouseX, float mouseY, float wheel);
            void OnMouseMove(float mouseX, float mouseY);
            void OnKeyboardKey(uint8_t keyID, bool pressed);

            void Render(gfx::TextureHandle target, util::CommandBuffer& graphicCommands);

        private:
            void ConstructFrame();

            gfx::Device* m_Device;

            gfx::TextureHandle m_DefaultFont;
            math::Matrix4x4 m_GuiProjectionMatrix;

            gfx::RenderState* m_GuiDrawRenderState;

            bool m_Visible;

            util::BufferArray<Tool*> m_Tools;
        };
    }
}
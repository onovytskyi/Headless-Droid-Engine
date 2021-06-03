#include "Engine/Config/Bootstrap.h"

#include "Engine/Engine/DebugOverlay/DebugOverlay.h"

#include "Engine/Debug/Assert.h"
#include "Engine/Framework/Graphics/Device.h"
#include "Engine/Framework/Graphics/GraphicCommands.h"
#include "Engine/Framework/Graphics/RenderState.h"
#include "Engine/Framework/Memory/AllocationScope.h"

namespace hd
{
    namespace ui
    {
        DebugOverlay::DebugOverlay(gfx::Backend& backend, gfx::Device& device, uint32_t width, uint32_t height, gfx::GraphicFormat targetFormat, mem::AllocationScope& allocationScope, 
            size_t maxTools)
            : m_Device{ &device }
            , m_DefaultFont{}
            , m_GuiProjectionMatrix{}
            , m_GuiDrawRenderState{}
            , m_Visible{ false }
            , m_Tools{ allocationScope, maxTools }
        {
            ImGui::CreateContext();
            ImGui::StyleColorsDark();
            ImGui::GetStyle().WindowRounding = 5;
            ImGui::GetStyle().FrameRounding = 5;
            ImGui::GetStyle().GrabRounding = 5;
            ImGui::GetStyle().PopupRounding = 5;
            ImGui::GetStyle().ScrollbarRounding = 5;
            ImGui::GetStyle().ChildRounding = 5;

            ImGuiIO& imguiIO = ImGui::GetIO();

            imguiIO.IniFilename = nullptr;

            m_GuiDrawRenderState = allocationScope.AllocateObject<gfx::RenderState>(backend);
            m_GuiDrawRenderState->SetVS(u8"DrawGui.hlsl", u8"MainVS");
            m_GuiDrawRenderState->SetPS(u8"DrawGui.hlsl", u8"MainPS");
            m_GuiDrawRenderState->SetRenderTargetFormat(0, targetFormat);
            m_GuiDrawRenderState->SetDepthEnable(false);
            m_GuiDrawRenderState->SetStencilEnable(false);
            m_GuiDrawRenderState->SetPrimitiveType(gfx::PrimitiveType::Triangle);
            m_GuiDrawRenderState->SetBlendType(gfx::BlendType::Alpha, gfx::BlendType::None);

            Resize(width, height);
        }

        DebugOverlay::~DebugOverlay()
        {
            m_Device->DestroyTexture(m_DefaultFont);
        }

        void DebugOverlay::UploadFont(hd::util::CommandBuffer& commandBuffer)
        {
            ImGuiIO& imguiIO = ImGui::GetIO();

            uint8_t* fontTextureData;
            int32_t fontTextureWidth;
            int32_t fontTextureHeight;
            int32_t fontTextureBPP;

            imguiIO.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Consola.ttf", 13.0f);
            imguiIO.Fonts->GetTexDataAsRGBA32(&fontTextureData, &fontTextureWidth, &fontTextureHeight, &fontTextureBPP);

            m_DefaultFont = m_Device->CreateTexture(fontTextureWidth, fontTextureHeight, 1, 1, gfx::GraphicFormat::RGBA8UNorm, uint32_t(gfx::TextureFlags::ShaderResource),
                gfx::TextureDimenstion::Texture2D, nullptr);

            gfx::GraphicCommandsStream commandStream{ commandBuffer };
            commandStream.UpdateTexture(m_DefaultFont, 0, gfx::ALL_SUBRESOURCES, fontTextureData, size_t(fontTextureWidth) * fontTextureHeight * fontTextureBPP);

            imguiIO.Fonts->SetTexID(uint64_t(m_DefaultFont));
        }

        void DebugOverlay::RegisterDebugTool(Tool& tool)
        {
            m_Tools.Add(&tool);

            Tool** firstTool = &m_Tools[0];
            Tool** pastLastTool = firstTool + m_Tools.GetSize();
            std::stable_sort(firstTool, pastLastTool, [](const Tool* a, const Tool* b) {
                return strcmp(a->GetMenuName(), b->GetMenuName()) > 0;
                });
        }

        void DebugOverlay::Resize(uint32_t width, uint32_t height)
        {
            ImGuiIO& imguiIO = ImGui::GetIO();

            uint32_t oldWidth = uint32_t(imguiIO.DisplaySize.x);
            uint32_t oldHeight = uint32_t(imguiIO.DisplaySize.y);

            if (oldWidth != width || oldHeight != height)
            {
                imguiIO.DisplaySize = { float(width), float(height) };

                m_GuiProjectionMatrix =
                {
                    2.0f / float(width),             0.0f,                                0.0f, 0.0f ,
                    0.0f,                            2.0f / -float(height),               0.0f, 0.0f ,
                    0.0f,                            0.0f,                                0.5f, 0.0f ,
                    -1.0f,                           1.0f,                                0.5f, 1.0f
                };
            }
        }

        void DebugOverlay::OnMouseButon(float mouseX, float mouseY, uint32_t mouseButtonId, bool pressed)
        {
            ImGuiIO& imguiIO = ImGui::GetIO();

            imguiIO.MousePos.x = mouseX;
            imguiIO.MousePos.y = mouseY;

            imguiIO.MouseDown[mouseButtonId] = pressed;
        }

        void DebugOverlay::OnMouseWheel(float mouseX, float mouseY, float wheel)
        {
            ImGuiIO& imguiIO = ImGui::GetIO();

            imguiIO.MousePos.x = mouseX;
            imguiIO.MousePos.y = mouseY;

            imguiIO.MouseWheel = wheel;
        }

        void DebugOverlay::OnMouseMove(float mouseX, float mouseY)
        {
            ImGuiIO& imguiIO = ImGui::GetIO();

            imguiIO.MousePos.x = mouseX;
            imguiIO.MousePos.y = mouseY;
        }

        void DebugOverlay::OnKeyboardKey(uint8_t keyID, bool pressed)
        {
            // #HACK windows key codes here for now. Ideally they should be converted to platform agnostic enum.
            static const uint8_t SHIFT_ID   = 0x10; // VK_SHIFT key code
            static const uint8_t CTRL_ID    = 0x11; // VK_CONTROL key code
            static const uint8_t ALT_ID     = 0x12; // VK_MENU key code
            static const uint8_t RWIN_ID    = 0x5C; // VK_RWIN key code
            static const uint8_t LWIN_ID    = 0x5B; // VK_LWIN key code
            static const uint8_t F9_ID      = 0x78; // VK_F9 key code

            ImGuiIO& imguiIO = ImGui::GetIO();

            if (keyID == CTRL_ID)
            {
                imguiIO.KeyCtrl = pressed;
            }
            else if (keyID == ALT_ID)
            {
                imguiIO.KeyAlt = pressed;
            }
            else if (keyID == SHIFT_ID)
            {
                imguiIO.KeyShift = pressed;
            }
            else if (keyID == LWIN_ID || keyID == RWIN_ID)
            {
                imguiIO.KeySuper = pressed;
            }
            else if (keyID < 512)
            {
                imguiIO.KeysDown[keyID] = pressed;
            }

            if (keyID == F9_ID && pressed)
            {
                m_Visible = !m_Visible;
            }
        }

        void DebugOverlay::Render(gfx::TextureHandle target, util::CommandBuffer& graphicCommands)
        {
            hd::gfx::GraphicCommandsStream commandStream{ graphicCommands };

            ConstructFrame();

            ImGuiIO& imguiIO = ImGui::GetIO();
            ImDrawData* drawData = ImGui::GetDrawData();
            if (drawData && drawData->CmdListsCount > 0)
            {
                uint32_t indexCount = 0;
                uint32_t vertexCount = 0;
                for (int32_t cmdListIdx = 0; cmdListIdx < drawData->CmdListsCount; ++cmdListIdx)
                {
                    ImDrawList* cmdList = drawData->CmdLists[cmdListIdx];
                    indexCount += cmdList->IdxBuffer.size();
                    vertexCount += cmdList->VtxBuffer.size();
                }

                gfx::BufferHandle indexBuffer = m_Device->CreateBuffer(indexCount, uint32_t(sizeof(ImDrawIdx)),
                    uint32_t(gfx::BufferFlags::ShaderResource) | uint32_t(gfx::BufferFlags::Transient));

                gfx::BufferHandle vertexBuffer = m_Device->CreateBuffer(vertexCount, uint32_t(sizeof(ImDrawVert)),
                    uint32_t(gfx::BufferFlags::ShaderResource) | uint32_t(gfx::BufferFlags::Transient));

                uint32_t commandListBaseIndex = 0;
                uint32_t commandListBaseVertex = 0;
                for (int32_t cmdListIdx = 0; cmdListIdx < drawData->CmdListsCount; ++cmdListIdx)
                {
                    ImDrawList* cmdList = drawData->CmdLists[cmdListIdx];

                    commandStream.UpdateBuffer(indexBuffer, commandListBaseIndex * sizeof(ImDrawIdx), cmdList->IdxBuffer.Data, cmdList->IdxBuffer.size() * sizeof(ImDrawIdx));
                    commandStream.UpdateBuffer(vertexBuffer, commandListBaseVertex * sizeof(ImDrawVert), cmdList->VtxBuffer.Data, cmdList->VtxBuffer.size() * sizeof(ImDrawVert));

                    commandListBaseIndex += cmdList->IdxBuffer.size();
                    commandListBaseVertex += cmdList->VtxBuffer.size();
                }

                gfx::BufferHandle constantBuffer = m_Device->CreateBuffer(1, sizeof(m_GuiProjectionMatrix), 
                    uint32_t(gfx::BufferFlags::ConstantBuffer) | uint32_t(gfx::BufferFlags::Transient));
                math::Matrix4x4 gpuGuiProjectionMatrix = math::MatrixTranspose(m_GuiProjectionMatrix);
                commandStream.UpdateBuffer(constantBuffer, 0, gpuGuiProjectionMatrix.m, sizeof(gpuGuiProjectionMatrix));

                commandStream.SetRenderState(m_GuiDrawRenderState);
                commandStream.SetTopologyType(gfx::TopologyType::List);
                commandStream.SetViewport({ 0.0f, imguiIO.DisplaySize.x, 0.0f, imguiIO.DisplaySize.y, 0.0f, 1.0f });
                commandStream.SetRenderTarget(target);

                commandStream.SetRootVariable(0, m_Device->GetCBVShaderIndex(constantBuffer));
                commandStream.SetRootVariable(1, m_Device->GetSRVShaderIndex(indexBuffer));
                commandStream.SetRootVariable(2, m_Device->GetSRVShaderIndex(vertexBuffer));

                commandListBaseIndex = 0;
                commandListBaseVertex = 0;
                for (int32_t cmdListIdx = 0; cmdListIdx < drawData->CmdListsCount; ++cmdListIdx)
                {
                    ImDrawList* cmdList = drawData->CmdLists[cmdListIdx];
                    for (ImDrawCmd& renderCmd : cmdList->CmdBuffer)
                    {
                        commandStream.SetScissorRect({ uint32_t(renderCmd.ClipRect.x), uint32_t(renderCmd.ClipRect.z), uint32_t(renderCmd.ClipRect.y), uint32_t(renderCmd.ClipRect.w) });

                        commandStream.SetRootVariable(3, commandListBaseIndex + renderCmd.IdxOffset);
                        commandStream.SetRootVariable(4, commandListBaseVertex + renderCmd.VtxOffset);

                        if (renderCmd.TextureId != std::numeric_limits<uint64_t>::max())
                        {
                            commandStream.UseAsReadableResource(gfx::TextureHandle(renderCmd.TextureId));
                            commandStream.SetRootVariable(5, m_Device->GetSRVShaderIndex(gfx::TextureHandle(renderCmd.TextureId)));
                        }
                        else
                        {
                            commandStream.SetRootVariable(5, uint32_t(-1));
                        }

                        commandStream.DrawInstanced(renderCmd.ElemCount, 1);
                    }

                    commandListBaseIndex += cmdList->IdxBuffer.size();
                    commandListBaseVertex += cmdList->VtxBuffer.size();
                }

                m_Device->DestroyBuffer(constantBuffer);
                m_Device->DestroyBuffer(indexBuffer);
                m_Device->DestroyBuffer(vertexBuffer);
            }
        }

        void DebugOverlay::ConstructFrame()
        {
            ImGui::NewFrame();

            if (m_Visible)
            {
                if (ImGui::BeginMainMenuBar())
                {
                    for (size_t toolIdx = 0; toolIdx < m_Tools.GetSize(); ++toolIdx)
                    {
                        Tool* tool = m_Tools[toolIdx];
                        if (!tool->IsAlwaysVisible())
                        {
                            if (ImGui::BeginMenu(tool->GetMenuName()))
                            {
                                ImGui::MenuItem(tool->GetToolName(), nullptr, nullptr, &tool->GetVisibleRef());
                                ImGui::EndMenu();
                            }
                        }
                    }

                    ImGui::EndMainMenuBar();
                }

                for (size_t toolIdx = 0; toolIdx < m_Tools.GetSize(); ++toolIdx)
                {
                    Tool* tool = m_Tools[toolIdx];

                    if (tool->GetVisibleRef() && !tool->IsAlwaysVisible())
                    {
                        tool->Draw();
                    }
                }
            }

            for (size_t toolIdx = 0; toolIdx < m_Tools.GetSize(); ++toolIdx)
            {
                if (m_Tools[toolIdx]->IsAlwaysVisible())
                {
                    m_Tools[toolIdx]->Draw();
                }
            }

            ImGui::Render();
        }
    }
}

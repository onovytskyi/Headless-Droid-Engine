#pragma once

#include "Engine/DebugOverlay/DebugOverlay.h"
#include "Engine/DebugOverlay/Tools/CommandsDebugTool.h"
#include "Engine/DebugOverlay/Tools/StatsDebugTool.h"
#include "Engine/Render/MeshCollection.h"
#include "Engine/Render/TextureCollection.h"
#include "Engine/Scene/FpsCamera.h"
#include "Framework/Graphics/Backend.h"
#include "Framework/Graphics/Device.h"
#include "Framework/Graphics/Queue.h"
#include "Framework/Graphics/Swapchain.h"
#include "Framework/System/SystemCommands.h"
#include "Framework/System/SystemWindow.h"
#include "Framework/System/Timer.h"
#include "Framework/Utils/CommandBuffer.h"

class GBufferPass;
class LightingPass;

class SampleGame
{
public:
    SampleGame();
    ~SampleGame();

    void Run();
    void RequestExit();

private:
    void PrepareResouces();
    void ProcessSystemCommands();
    void RenderFrame();
    void RecordFrame(hd::util::CommandBuffer& commandBuffer);

    bool m_IsRunning;

    hd::sys::Timer* m_FrameTimer;

    hd::sys::SystemWindow* m_MainWindow;
    hd::util::CommandBuffer m_SystemCommands;

    hd::gfx::Backend* m_GfxBackend;
    hd::gfx::Device* m_GfxDevice;
    hd::gfx::Queue* m_GfxQueue;
    hd::gfx::Swapchain* m_GfxSwapchain;

    hd::ui::DebugOverlay* m_DebugOverlay;
    hd::ui::StatsDebugTool* m_StatsDebugTool;
    hd::ui::CommandsDebugTool* m_CommandsDebugTool;

    hd::util::CommandBuffer m_GraphicCommands;

    hd::render::MeshCollection* m_SceneMeshCollection;
    hd::render::TextureCollection* m_SceneTextureCollection;

    hd::scene::FpsCamera* m_FpsCamera;

    GBufferPass* m_GBufferPass;
    LightingPass* m_LightingPass;
};
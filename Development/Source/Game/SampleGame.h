#pragma once

#include "Engine/Engine/DebugOverlay/DebugOverlay.h"
#include "Engine/Engine/DebugOverlay/Tools/CommandsDebugTool.h"
#include "Engine/Engine/DebugOverlay/Tools/StatsDebugTool.h"
#include "Engine/Engine/Render/MeshCollection.h"
#include "Engine/Engine/Render/TextureCollection.h"
#include "Engine/Engine/Scene/FpsCamera.h"
#include "Engine/Framework/Graphics/Backend.h"
#include "Engine/Framework/Graphics/Device.h"
#include "Engine/Framework/Graphics/Queue.h"
#include "Engine/Framework/Graphics/Swapchain.h"
#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/System/SystemCommands.h"
#include "Engine/Framework/System/SystemWindow.h"
#include "Engine/Framework/System/Timer.h"
#include "Engine/Framework/Utils/CommandBuffer.h"

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

    hd::mem::AllocationScope m_PersistentScope;

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
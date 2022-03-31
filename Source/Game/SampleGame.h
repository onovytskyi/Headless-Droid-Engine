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
    void RecordFrame(hd::CommandBuffer& commandBuffer);

    bool m_IsRunning;

    hd::Timer* m_FrameTimer;

    hd::SystemWindow* m_MainWindow;
    hd::CommandBuffer m_SystemCommands;

    hd::Backend* m_GfxBackend;
    hd::Device* m_GfxDevice;
    hd::Queue* m_GfxQueue;
    hd::Swapchain* m_GfxSwapchain;

    hd::DebugOverlay* m_DebugOverlay;
    hd::StatsDebugTool* m_StatsDebugTool;
    hd::CommandsDebugTool* m_CommandsDebugTool;

    hd::CommandBuffer m_GraphicCommands;

    hd::MeshCollection* m_SceneMeshCollection;
    hd::TextureCollection* m_SceneTextureCollection;

    hd::FpsCamera* m_FpsCamera;

    GBufferPass* m_GBufferPass;
    LightingPass* m_LightingPass;
};
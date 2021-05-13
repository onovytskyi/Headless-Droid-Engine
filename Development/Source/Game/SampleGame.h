#pragma once

#include "Engine/Framework/Graphics/Backend.h"
#include "Engine/Framework/Graphics/Device.h"
#include "Engine/Framework/Graphics/Queue.h"
#include "Engine/Framework/Graphics/Swapchain.h"
#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/System/SystemCommands.h"
#include "Engine/Framework/System/SystemWindow.h"
#include "Engine/Framework/Utils/CommandQueue.h"

class SampleGame
{
public:
    SampleGame();
    ~SampleGame();

    void Run();
    void RequestExit();

private:
    void ProcessSystemCommands();
    void RenderFrame();

    bool m_IsRunning;

    hd::mem::AllocationScope m_PersistentScope;

    hd::sys::SystemWindow* m_MainWindow;
    hd::util::CommandQueue m_SystemCommands;

    hd::gfx::Backend* m_GfxBackend;
    hd::gfx::Device* m_GfxDevice;
    hd::gfx::Queue* m_GfxQueue;
    hd::gfx::Swapchain* m_GfxSwapchain;
};
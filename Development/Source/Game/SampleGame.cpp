#include "Game/Bootstrap.h"

#include "Game/SampleGame.h"

#include "Engine/Debug/Log.h"
#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Foundation/Memory/Utils.h"
#include "Engine/Framework/Graphics/GraphicsTypes.h"

SampleGame::SampleGame()
    : m_IsRunning{ false }
    , m_PersistentScope{ hd::mem::GetPersistentAllocator() }
    , m_MainWindow{}
    , m_SystemCommands{ hd::mem::MB(64) }
    , m_GfxBackend{}
    , m_GfxDevice{}
    , m_GfxQueue{}
    , m_GfxSwapchain{}
{
    m_MainWindow = m_PersistentScope.AllocateObject<hd::sys::SystemWindow>(u8"Droid Engine : Sample Game", 1280, 720);
    m_GfxBackend = m_PersistentScope.AllocateObject<hd::gfx::Backend>();
    m_GfxDevice = m_PersistentScope.AllocateObject<hd::gfx::Device>(*m_GfxBackend, m_PersistentScope);
    m_GfxQueue = m_PersistentScope.AllocateObject<hd::gfx::Queue>(*m_GfxDevice, hd::gfx::QueueType::Graphics, m_PersistentScope);
    m_GfxSwapchain = m_PersistentScope.AllocateObject<hd::gfx::Swapchain>(*m_GfxBackend, *m_GfxDevice, *m_GfxQueue, *m_MainWindow, hd::gfx::GraphicFormat::RGBA8UNorm_Srgb, m_PersistentScope);
}

SampleGame::~SampleGame()
{

}

void SampleGame::Run()
{
    hdLogInfo(u8"Starting game engine loop.");

    m_MainWindow->SetVisible(true);

    m_IsRunning = true;
    while (m_IsRunning)
    {
        hd::mem::AllocationScope frameScope(hd::mem::GetFrameAllocator());

        m_MainWindow->ProcessSystemEvents(m_SystemCommands);

        ProcessSystemCommands();
        RenderFrame();
    }
}

void SampleGame::RequestExit()
{
    m_IsRunning = false;
}

void SampleGame::ProcessSystemCommands()
{
    while (m_SystemCommands.HasCommands())
    {
        hd::sys::SystemCommandType& commandType = m_SystemCommands.Pop<hd::sys::SystemCommandType>();

        switch (commandType)
        {
        case hd::sys::SystemCommandType::WindowActivate:
        {
            hd::sys::WindowActivateCommand::PopFrom(m_SystemCommands);
        }
        break;
        case hd::sys::SystemCommandType::WindowClosed:
        {
            hd::sys::WindowClosedCommand::PopFrom(m_SystemCommands);
            RequestExit();
        }
        break;
        case hd::sys::SystemCommandType::WindowResize:
        {
            hd::sys::WindowResizeCommand& command = hd::sys::WindowResizeCommand::PopFrom(m_SystemCommands);
            m_GfxSwapchain->Resize(command.Width, command.Height);
        }
        break;
        case hd::sys::SystemCommandType::MouseButton:
        {
            hd::sys::MouseButtonCommand::PopFrom(m_SystemCommands);
        }
        break;
        case hd::sys::SystemCommandType::MouseMove:
        {
            hd::sys::MouseMoveCommand::PopFrom(m_SystemCommands);
        }
        break;
        case hd::sys::SystemCommandType::MouseWheel:
        {
            hd::sys::MouseWheelCommand::PopFrom(m_SystemCommands);
        }
        break;
        case hd::sys::SystemCommandType::Keyboard:
        {
            hd::sys::KeyboardCommand& command = hd::sys::KeyboardCommand::PopFrom(m_SystemCommands);
            if (command.Pressed && command.KeyID == VK_ESCAPE)
            {
                RequestExit();
            }
        }
        break;
        default:
            hdAssert(false, u8"Cannot process command. Unknown command type.");
            break;
        }
    }

    m_SystemCommands.Clear();
}

void SampleGame::RenderFrame()
{
    m_GfxSwapchain->Flip();

    m_GfxDevice->RecycleResources(m_GfxSwapchain->GetCPUFrame(), m_GfxSwapchain->GetGPUFrame());
}

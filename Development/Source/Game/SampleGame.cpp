#include "Game/Bootstrap.h"

#include "Game/SampleGame.h"

#include "Engine/Debug/Log.h"
#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Foundation/Memory/Utils.h"
#include "Engine/Framework/Graphics/GraphicsTypes.h"
#include "Engine/Framework/Utils/CommandBufferReader.h"

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
    hd::util::CommandBufferReader systemCommandsReader{ m_SystemCommands };

    while (systemCommandsReader.HasCommands())
    {
        hd::sys::SystemCommandType& commandType = systemCommandsReader.Read<hd::sys::SystemCommandType>();

        switch (commandType)
        {
        case hd::sys::SystemCommandType::WindowActivate:
        {
            hd::sys::WindowActivateCommand::ReadFrom(systemCommandsReader);
        }
        break;
        case hd::sys::SystemCommandType::WindowClosed:
        {
            hd::sys::WindowClosedCommand::ReadFrom(systemCommandsReader);
            RequestExit();
        }
        break;
        case hd::sys::SystemCommandType::WindowResize:
        {
            hd::sys::WindowResizeCommand& command = hd::sys::WindowResizeCommand::ReadFrom(systemCommandsReader);
            m_GfxSwapchain->Resize(command.Width, command.Height);
        }
        break;
        case hd::sys::SystemCommandType::MouseButton:
        {
            hd::sys::MouseButtonCommand::ReadFrom(systemCommandsReader);
        }
        break;
        case hd::sys::SystemCommandType::MouseMove:
        {
            hd::sys::MouseMoveCommand::ReadFrom(systemCommandsReader);
        }
        break;
        case hd::sys::SystemCommandType::MouseWheel:
        {
            hd::sys::MouseWheelCommand::ReadFrom(systemCommandsReader);
        }
        break;
        case hd::sys::SystemCommandType::Keyboard:
        {
            hd::sys::KeyboardCommand& command = hd::sys::KeyboardCommand::ReadFrom(systemCommandsReader);
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
     hd::gfx::TextureHandle framebuffer{};
     framebuffer = m_GfxSwapchain->GetActiveFramebuffer();

    m_GfxSwapchain->Flip();

    m_GfxDevice->RecycleResources(m_GfxSwapchain->GetCPUFrame(), m_GfxSwapchain->GetGPUFrame());
}

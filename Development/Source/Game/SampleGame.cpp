#include "Game/Bootstrap.h"

#include "Game/SampleGame.h"

#include "Engine/Debug/Log.h"
#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Foundation/Memory/Utils.h"
#include "Engine/Framework/Graphics/GraphicCommands.h"
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
    , m_DebugOverlay{}
    , m_GraphicCommands{ hd::mem::MB(64) }
{
    const hd::gfx::GraphicFormat backBufferFormat = hd::gfx::GraphicFormat::RGBA8UNorm_Srgb;

    m_FrameTimer = m_PersistentScope.AllocateObject<hd::sys::Timer>();
    m_MainWindow = m_PersistentScope.AllocateObject<hd::sys::SystemWindow>(u8"Droid Engine : Sample Game", 1280, 720);
    m_GfxBackend = m_PersistentScope.AllocateObject<hd::gfx::Backend>(m_PersistentScope);
    m_GfxDevice = m_PersistentScope.AllocateObject<hd::gfx::Device>(*m_GfxBackend, m_PersistentScope);
    m_GfxQueue = m_PersistentScope.AllocateObject<hd::gfx::Queue>(*m_GfxDevice, hd::gfx::QueueType::Graphics, m_PersistentScope);
    m_GfxSwapchain = m_PersistentScope.AllocateObject<hd::gfx::Swapchain>(*m_GfxBackend, *m_GfxDevice, *m_GfxQueue, *m_MainWindow, backBufferFormat, m_PersistentScope);
    m_DebugOverlay = m_PersistentScope.AllocateObject<hd::ui::DebugOverlay>(*m_GfxBackend, *m_GfxDevice, m_MainWindow->GetWidth(), m_MainWindow->GetHeight(), backBufferFormat, 
        m_PersistentScope, 64);
    m_StatsDebugTool = m_PersistentScope.AllocateObject<hd::ui::StatsDebugTool>();

    m_DebugOverlay->RegisterDebugTool(*m_StatsDebugTool);
}

SampleGame::~SampleGame()
{
    m_GfxQueue->Flush();

    m_FrameTimer->Tick();
    hdLogInfo(u8"Engine shutdown took % seconds.", m_FrameTimer->GetDeltaSeconds());
}

void SampleGame::Run()
{
    hdLogInfo(u8"Starting game engine loop.");

    PrepareResouces();

    m_MainWindow->SetVisible(true);

    m_FrameTimer->Tick();
    hdLogInfo(u8"Engine initialization took % seconds.", m_FrameTimer->GetDeltaSeconds());

    m_IsRunning = true;
    while (m_IsRunning)
    {
        m_FrameTimer->Tick();
        m_StatsDebugTool->Update(*m_FrameTimer, *m_GfxSwapchain, *m_GfxDevice);

        hd::mem::AllocationScope frameScope(hd::mem::GetFrameAllocator());

        m_MainWindow->ProcessSystemEvents(m_SystemCommands);

        ProcessSystemCommands();
        RenderFrame();
    }
}

void SampleGame::RequestExit()
{
    m_IsRunning = false;

    // Update timer here so we can measure engine shutdown time
    m_FrameTimer->Tick();
}

void SampleGame::PrepareResouces()
{
    m_DebugOverlay->UploadFont(m_GraphicCommands);

    m_GfxQueue->Submit(m_GraphicCommands);
    m_GraphicCommands.Clear();
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
            m_DebugOverlay->Resize(command.Width, command.Height);
        }
        break;
        case hd::sys::SystemCommandType::MouseButton:
        {
            hd::sys::MouseButtonCommand& command = hd::sys::MouseButtonCommand::ReadFrom(systemCommandsReader);
            m_DebugOverlay->OnMouseButon(float(command.X), float(command.Y), command.ButtonID, command.Pressed);
        }
        break;
        case hd::sys::SystemCommandType::MouseMove:
        {
            hd::sys::MouseMoveCommand& command = hd::sys::MouseMoveCommand::ReadFrom(systemCommandsReader);
            m_DebugOverlay->OnMouseMove(float(command.X), float(command.Y));
        }
        break;
        case hd::sys::SystemCommandType::MouseWheel:
        {
            hd::sys::MouseWheelCommand& command = hd::sys::MouseWheelCommand::ReadFrom(systemCommandsReader);
            m_DebugOverlay->OnMouseWheel(float(command.X), float(command.Y), float(command.Offset));
        }
        break;
        case hd::sys::SystemCommandType::Keyboard:
        {
            hd::sys::KeyboardCommand& command = hd::sys::KeyboardCommand::ReadFrom(systemCommandsReader);
            if (command.Pressed && command.KeyID == VK_ESCAPE)
            {
                RequestExit();
            }

            m_DebugOverlay->OnKeyboardKey(command.KeyID, command.Pressed);
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
    RecordFrame(m_GraphicCommands);

    m_GfxQueue->Submit(m_GraphicCommands);
    m_GraphicCommands.Clear();

    m_GfxSwapchain->Flip();

    m_GfxDevice->RecycleResources(m_GfxSwapchain->GetCPUFrame(), m_GfxSwapchain->GetGPUFrame());
}

void SampleGame::RecordFrame(hd::util::CommandBuffer& commandBuffer)
{
    hd::gfx::TextureHandle framebuffer{};
    framebuffer = m_GfxSwapchain->GetActiveFramebuffer();

    hd::gfx::GraphicCommandsStream commandStream{ m_GraphicCommands };
    commandStream.ClearRenderTarget(framebuffer, { 0.0f, 0.0f, 1.0f, 1.0f });

    m_DebugOverlay->Render(framebuffer, m_GraphicCommands);
}

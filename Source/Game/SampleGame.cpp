#include "Game/Bootstrap.h"

#include "Game/SampleGame.h"

#include "Debug/Log.h"
#include "Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Utils/ResourceLoader.h"
#include "Foundation/Memory/Utils.h"
#include "Framework/Graphics/GraphicCommands.h"
#include "Framework/Graphics/GraphicsTypes.h"
#include "Framework/Math/Math.h"
#include "Framework/Utils/CommandBufferReader.h"
#include "Game/RenderPasses/GBufferPass.h"
#include "Game/RenderPasses/LightingPass.h"

SampleGame::SampleGame()
    : m_IsRunning{ false }
    , m_MainWindow{}
    , m_SystemCommands{ hd::MB(64) }
    , m_GfxBackend{}
    , m_GfxDevice{}
    , m_GfxQueue{}
    , m_GfxSwapchain{}
    , m_DebugOverlay{}
    , m_StatsDebugTool{}
    , m_CommandsDebugTool{}
    , m_GraphicCommands{ hd::MB(128) }
    , m_SceneMeshCollection{}
    , m_SceneTextureCollection{}
    , m_FpsCamera{}
    , m_GBufferPass{}
{
    const hd::GraphicFormat backBufferFormat = hd::GraphicFormat::RGBA8UNorm_Srgb;

    m_FrameTimer = hdNew(hd::Persistent(), hd::Timer)();
    m_MainWindow = hdNew(hd::Persistent(), hd::SystemWindow)(u8"Droid Engine : Sample Game", 1280, 720);
    m_GfxBackend = hdNew(hd::Persistent(), hd::Backend)(hd::Persistent());
    m_GfxDevice = hdNew(hd::Persistent(), hd::Device)(hd::Persistent(), hd::General(), *m_GfxBackend);
    m_GfxQueue = hdNew(hd::Persistent(), hd::Queue)(hd::Persistent(), *m_GfxDevice, hd::QueueType::Graphics);
    m_GfxSwapchain = hdNew(hd::Persistent(), hd::Swapchain)(hd::Persistent(), *m_GfxBackend, *m_GfxDevice, *m_GfxQueue, *m_MainWindow, backBufferFormat);
    m_DebugOverlay = hdNew(hd::Persistent(), hd::DebugOverlay)(*m_GfxBackend, *m_GfxDevice, m_MainWindow->GetWidth(), m_MainWindow->GetHeight(), backBufferFormat);
    m_StatsDebugTool = hdNew(hd::Persistent(), hd::StatsDebugTool)();
    m_CommandsDebugTool = hdNew(hd::Persistent(), hd::CommandsDebugTool)(*m_GfxDevice);

    m_FpsCamera = hdNew(hd::Persistent(), hd::FpsCamera)();
    m_FpsCamera->SetFPSCameraLens(hd::ConvertToRadians(60.0f), float(m_MainWindow->GetWidth()) / m_MainWindow->GetHeight(), 10000.0f, 0.1f);

    m_GBufferPass = hdNew(hd::Persistent(), GBufferPass)(*m_GfxBackend, *m_GfxDevice, m_MainWindow->GetWidth(), m_MainWindow->GetHeight());
    m_LightingPass = hdNew(hd::Persistent(), LightingPass)(*m_GfxBackend, *m_GfxDevice, backBufferFormat);

    m_DebugOverlay->RegisterDebugTool(*m_StatsDebugTool);
    m_DebugOverlay->RegisterDebugTool(*m_CommandsDebugTool);
}

SampleGame::~SampleGame()
{
    m_GfxQueue->Flush();

    hdSafeDelete(hd::Persistent(), m_SceneMeshCollection);
    hdSafeDelete(hd::Persistent(), m_SceneTextureCollection);
    hdSafeDelete(hd::Persistent(), m_LightingPass);
    hdSafeDelete(hd::Persistent(), m_GBufferPass);
    hdSafeDelete(hd::Persistent(), m_FpsCamera);
    hdSafeDelete(hd::Persistent(), m_CommandsDebugTool);
    hdSafeDelete(hd::Persistent(), m_StatsDebugTool);
    hdSafeDelete(hd::Persistent(), m_DebugOverlay);
    hdSafeDelete(hd::Persistent(), m_GfxSwapchain);
    hdSafeDelete(hd::Persistent(), m_GfxQueue);
    hdSafeDelete(hd::Persistent(), m_GfxDevice);
    hdSafeDelete(hd::Persistent(), m_GfxBackend);
    hdSafeDelete(hd::Persistent(), m_MainWindow);

    m_FrameTimer->Tick();
    hdLogInfo(u8"Engine shutdown took % seconds.", m_FrameTimer->GetDeltaSeconds());

    hdSafeDelete(hd::Persistent(), m_FrameTimer);
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

        m_MainWindow->ProcessSystemEvents(m_SystemCommands);

        ProcessSystemCommands();

        m_FpsCamera->UpdateCamera(*m_FrameTimer);
        m_StatsDebugTool->Update(*m_FrameTimer, *m_GfxSwapchain, *m_GfxDevice);

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
    hd::ScopedScratchMemory scopedScratch{};

    // Starting point for sponza
    m_FpsCamera->SetPosition({ 698.0f, 583.0f, -42.0f });
    m_FpsCamera->SetRotation({ 4.7f, 0.36f, 0.0f });

    std::pmr::vector<hd::MaterialResouce> materials{ &hd::Scratch() };
    std::pmr::vector<hd::MeshResource> meshes{ &hd::Scratch() };
    hd::LoadMesh(u8"Meshes/Sponza/sponza.obj", materials, meshes);

    std::pmr::vector<hd::MeshCollection::MaterialData> materialsToUpload{ materials.size(), &hd::Scratch() };
    std::pmr::vector<hd::MeshCollection::MeshData> meshesToUpload{ meshes.size(), &hd::Scratch() };

    m_SceneTextureCollection = hdNew(hd::Persistent(), hd::TextureCollection)(*m_GfxDevice);

    for (uint32_t materialIdx = 0; materialIdx < materials.size(); ++materialIdx)
    {
        materialsToUpload[materialIdx].DiffuseColor = materials[materialIdx].DiffuseColor;
        materialsToUpload[materialIdx].AmbientColor = materials[materialIdx].AmbientColor;
        materialsToUpload[materialIdx].SpecularColor = materials[materialIdx].SpecularColor;

        materialsToUpload[materialIdx].SpecularPower = materials[materialIdx].SpecularPower;
        materialsToUpload[materialIdx].FresnelR0 = (1.0f - materials[materialIdx].RefractionIndex) / (1.0f + materials[materialIdx].RefractionIndex);
        materialsToUpload[materialIdx].FresnelR0 *= materialsToUpload[materialIdx].FresnelR0;
        materialsToUpload[materialIdx].DiffuseTextureIdx = -1;
        materialsToUpload[materialIdx].NormalTextureIdx = -1;
        materialsToUpload[materialIdx].RoughnessTextureIdx = -1;
        materialsToUpload[materialIdx].MetalnessTextureIdx = -1;

        if (!materials[materialIdx].DiffuseTexture.empty())
        {
            hd::TextureHandle diffuseTexture = m_SceneTextureCollection->UploadTexture(materials[materialIdx].DiffuseTexture.c_str(), m_GraphicCommands);
            materialsToUpload[materialIdx].DiffuseTextureIdx = m_GfxDevice->GetSRVShaderIndex(diffuseTexture);
        }

        if (!materials[materialIdx].NormalTexture.empty())
        {
            hd::TextureHandle normalTexture = m_SceneTextureCollection->UploadTexture(materials[materialIdx].NormalTexture.c_str(), m_GraphicCommands);
            materialsToUpload[materialIdx].NormalTextureIdx = m_GfxDevice->GetSRVShaderIndex(normalTexture);
        }

        if (!materials[materialIdx].RoughnessTexture.empty())
        {
            hd::TextureHandle roughnessTexture = m_SceneTextureCollection->UploadTexture(materials[materialIdx].RoughnessTexture.c_str(), m_GraphicCommands);
            materialsToUpload[materialIdx].RoughnessTextureIdx = m_GfxDevice->GetSRVShaderIndex(roughnessTexture);
        }

        if (!materials[materialIdx].MetalnessTexture.empty())
        {
            hd::TextureHandle metalnessTexture = m_SceneTextureCollection->UploadTexture(materials[materialIdx].MetalnessTexture.c_str(), m_GraphicCommands);
            materialsToUpload[materialIdx].MetalnessTextureIdx = m_GfxDevice->GetSRVShaderIndex(metalnessTexture);
        }
    }

    for (uint32_t meshIdx = 0; meshIdx < meshes.size(); ++meshIdx)
    {
        meshesToUpload[meshIdx].Vertices = std::move(meshes[meshIdx].Vertices);
        meshesToUpload[meshIdx].Indices = std::move(meshes[meshIdx].Indices);
        meshesToUpload[meshIdx].MaterialIndex = meshes[meshIdx].MaterialIndex;
    }

    m_SceneMeshCollection = hdNew(hd::Persistent(), hd::MeshCollection)(*m_GfxDevice, m_GraphicCommands, materialsToUpload, meshesToUpload);

    m_DebugOverlay->UploadFont(m_GraphicCommands);

    m_GfxQueue->Submit(m_GraphicCommands);
    m_GraphicCommands.Clear();
}

void SampleGame::ProcessSystemCommands()
{
    hd::CommandBufferReader systemCommandsReader{ m_SystemCommands };

    while (systemCommandsReader.HasCommands())
    {
        hd::SystemCommandType& commandType = systemCommandsReader.Read<hd::SystemCommandType>();

        switch (commandType)
        {
        case hd::SystemCommandType::WindowActivate:
        {
            hd::WindowActivateCommand::ReadFrom(systemCommandsReader);
        }
        break;
        case hd::SystemCommandType::WindowClosed:
        {
            hd::WindowClosedCommand::ReadFrom(systemCommandsReader);
            RequestExit();
        }
        break;
        case hd::SystemCommandType::WindowResize:
        {
            hd::WindowResizeCommand& command = hd::WindowResizeCommand::ReadFrom(systemCommandsReader);
            m_GfxSwapchain->Resize(command.Width, command.Height);
            m_GBufferPass->ResizeTextures(command.Width, command.Height);
            m_DebugOverlay->Resize(command.Width, command.Height);
            m_FpsCamera->SetFPSCameraLens(hd::ConvertToRadians(60.0f), float(command.Width) / command.Height, 10000.0f, 0.1f);
        }
        break;
        case hd::SystemCommandType::MouseButton:
        {
            hd::MouseButtonCommand& command = hd::MouseButtonCommand::ReadFrom(systemCommandsReader);
            m_DebugOverlay->OnMouseButon(float(command.X), float(command.Y), command.ButtonID, command.Pressed);
            m_FpsCamera->OnMouseButon(float(command.X), float(command.Y), command.ButtonID, command.Pressed);
        }
        break;
        case hd::SystemCommandType::MouseMove:
        {
            hd::MouseMoveCommand& command = hd::MouseMoveCommand::ReadFrom(systemCommandsReader);
            m_DebugOverlay->OnMouseMove(float(command.X), float(command.Y));
        }
        break;
        case hd::SystemCommandType::MouseWheel:
        {
            hd::MouseWheelCommand& command = hd::MouseWheelCommand::ReadFrom(systemCommandsReader);
            m_DebugOverlay->OnMouseWheel(float(command.X), float(command.Y), float(command.Offset));
        }
        break;
        case hd::SystemCommandType::Keyboard:
        {
            hd::KeyboardCommand& command = hd::KeyboardCommand::ReadFrom(systemCommandsReader);
            if (command.Pressed && command.KeyID == VK_ESCAPE)
            {
                RequestExit();
            }

            m_DebugOverlay->OnKeyboardKey(command.KeyID, command.Pressed);
            m_FpsCamera->OnKeyboardKey(command.KeyID, command.Pressed);
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

void SampleGame::RecordFrame(hd::CommandBuffer& commandBuffer)
{
    hd::TextureHandle framebuffer = m_GfxSwapchain->GetActiveFramebuffer();

    hd::GraphicCommandsStream commandStream{ m_GraphicCommands };
    commandStream.ClearRenderTarget(framebuffer, { 0.0f, 0.0f, 1.0f, 1.0f });

    m_GBufferPass->RenderFrame(m_GraphicCommands, *m_FpsCamera, *m_SceneMeshCollection);
    m_LightingPass->RenderFrame(m_GraphicCommands, *m_FpsCamera, m_GBufferPass->GetSurface0(), m_GBufferPass->GetSurface1(), m_GBufferPass->GetSurface2(), m_GBufferPass->GetDepth(),
        framebuffer);

    m_DebugOverlay->Render(framebuffer, m_GraphicCommands);
}

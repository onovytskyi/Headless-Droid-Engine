#include "Game/Bootstrap.h"

#include "Game/SampleGame.h"

#include "Engine/Debug/Log.h"
#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Engine/Utils/ResourceLoader.h"
#include "Engine/Foundation/Memory/Utils.h"
#include "Engine/Framework/Graphics/GraphicCommands.h"
#include "Engine/Framework/Graphics/GraphicsTypes.h"
#include "Engine/Framework/Math/Math.h"
#include "Engine/Framework/Utils/CommandBufferReader.h"
#include "Game/RenderPasses/GBufferPass.h"
#include "Game/RenderPasses/LightingPass.h"

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
    , m_StatsDebugTool{}
    , m_CommandsDebugTool{}
    , m_GraphicCommands{ hd::mem::MB(128) }
    , m_SceneMeshCollection{}
    , m_SceneTextureCollection{}
    , m_FpsCamera{}
    , m_GBufferPass{}
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
    m_CommandsDebugTool = m_PersistentScope.AllocateObject<hd::ui::CommandsDebugTool>(*m_GfxDevice);
    m_SceneTextureCollection = m_PersistentScope.AllocateObject<hd::render::TextureCollection>(m_PersistentScope, *m_GfxDevice);

    m_FpsCamera = m_PersistentScope.AllocateObject<hd::scene::FpsCamera>();
    m_FpsCamera->SetFPSCameraLens(hd::math::ConvertToRadians(60.0f), float(m_MainWindow->GetWidth()) / m_MainWindow->GetHeight(), 10000.0f, 0.1f);

    m_GBufferPass = m_PersistentScope.AllocateObject<GBufferPass>(*m_GfxBackend, *m_GfxDevice, m_MainWindow->GetWidth(), m_MainWindow->GetHeight());
    m_LightingPass = m_PersistentScope.AllocateObject<LightingPass>(*m_GfxBackend, *m_GfxDevice, backBufferFormat);

    m_DebugOverlay->RegisterDebugTool(*m_StatsDebugTool);
    m_DebugOverlay->RegisterDebugTool(*m_CommandsDebugTool);
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

        hd::mem::AllocationScope frameScope(hd::mem::GetFrameAllocator());

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
    hd::mem::AllocationScope scratchScope{ hd::mem::GetScratchAllocator() };

    // Starting point for sponza
    m_FpsCamera->SetPosition({ 698.0f, 583.0f, -42.0f });
    m_FpsCamera->SetRotation({ 4.7f, 0.36f, 0.0f });

    hd::mem::Buffer materials{ scratchScope };
    hd::mem::Buffer meshes{ scratchScope };
    hd::util::LoadMesh(scratchScope, u8"Meshes/Sponza/sponza.obj", materials, meshes);

    hd::util::MaterialResouce* materialsArray = materials.GetDataAs<hd::util::MaterialResouce*>();
    uint32_t materialCount = uint32_t(materials.GetSize() / sizeof(hd::util::MaterialResouce));

    hd::util::MeshResource* meshesArray = meshes.GetDataAs<hd::util::MeshResource*>();
    uint32_t meshCount = uint32_t(meshes.GetSize() / sizeof(hd::util::MeshResource));

    hd::util::BufferArray<hd::render::MeshCollection::MaterialData> materialsToUpload{ scratchScope, materialCount };
    materialsToUpload.ResizeToMax();
    hd::util::BufferArray<hd::render::MeshCollection::MeshData> meshesToUpload{ scratchScope, meshCount };
    meshesToUpload.ResizeToMax();

    for (uint32_t materialIdx = 0; materialIdx < materialCount; ++materialIdx)
    {
        materialsToUpload[materialIdx].DiffuseColor = materialsArray[materialIdx].DiffuseColor;
        materialsToUpload[materialIdx].AmbientColor = materialsArray[materialIdx].AmbientColor;
        materialsToUpload[materialIdx].SpecularColor = materialsArray[materialIdx].SpecularColor;

        materialsToUpload[materialIdx].SpecularPower = materialsArray[materialIdx].SpecularPower;
        materialsToUpload[materialIdx].FresnelR0 = (1.0f - materialsArray[materialIdx].RefractionIndex) / (1.0f + materialsArray[materialIdx].RefractionIndex);
        materialsToUpload[materialIdx].FresnelR0 *= materialsToUpload[materialIdx].FresnelR0;
        materialsToUpload[materialIdx].DiffuseTextureIdx = -1;
        materialsToUpload[materialIdx].NormalTextureIdx = -1;
        materialsToUpload[materialIdx].RoughnessTextureIdx = -1;
        materialsToUpload[materialIdx].MetalnessTextureIdx = -1;

        if (materialsArray[materialIdx].DiffuseTexture)
        {
            hd::gfx::TextureHandle diffuseTexture = m_SceneTextureCollection->UploadTexture(materialsArray[materialIdx].DiffuseTexture, m_GraphicCommands);
            materialsToUpload[materialIdx].DiffuseTextureIdx = m_GfxDevice->GetSRVShaderIndex(diffuseTexture);
        }

        if (materialsArray[materialIdx].NormalTexture)
        {
            hd::gfx::TextureHandle normalTexture = m_SceneTextureCollection->UploadTexture(materialsArray[materialIdx].NormalTexture, m_GraphicCommands);
            materialsToUpload[materialIdx].NormalTextureIdx = m_GfxDevice->GetSRVShaderIndex(normalTexture);
        }

        if (materialsArray[materialIdx].RoughnessTexture)
        {
            hd::gfx::TextureHandle roughnessTexture = m_SceneTextureCollection->UploadTexture(materialsArray[materialIdx].RoughnessTexture, m_GraphicCommands);
            materialsToUpload[materialIdx].RoughnessTextureIdx = m_GfxDevice->GetSRVShaderIndex(roughnessTexture);
        }

        if (materialsArray[materialIdx].MetalnessTexture)
        {
            hd::gfx::TextureHandle metalnessTexture = m_SceneTextureCollection->UploadTexture(materialsArray[materialIdx].MetalnessTexture, m_GraphicCommands);
            materialsToUpload[materialIdx].MetalnessTextureIdx = m_GfxDevice->GetSRVShaderIndex(metalnessTexture);
        }
    }

    for (uint32_t meshIdx = 0; meshIdx < meshCount; ++meshIdx)
    {
        meshesToUpload[meshIdx].Vertices = meshesArray[meshIdx].Vertices;
        meshesToUpload[meshIdx].VertexCount = meshesArray[meshIdx].VertexCount;
        meshesToUpload[meshIdx].Indices = meshesArray[meshIdx].Indices;
        meshesToUpload[meshIdx].IndexCount = meshesArray[meshIdx].IndexCount;
        meshesToUpload[meshIdx].MaterialIndex = meshesArray[meshIdx].MaterialIndex;
    }

    m_SceneMeshCollection = m_PersistentScope.AllocateObject<hd::render::MeshCollection>(m_PersistentScope, *m_GfxDevice, m_GraphicCommands, materialsToUpload, meshesToUpload);

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
            m_GBufferPass->ResizeTextures(command.Width, command.Height);
            m_DebugOverlay->Resize(command.Width, command.Height);
            m_FpsCamera->SetFPSCameraLens(hd::math::ConvertToRadians(60.0f), float(command.Width) / command.Height, 10000.0f, 0.1f);
        }
        break;
        case hd::sys::SystemCommandType::MouseButton:
        {
            hd::sys::MouseButtonCommand& command = hd::sys::MouseButtonCommand::ReadFrom(systemCommandsReader);
            m_DebugOverlay->OnMouseButon(float(command.X), float(command.Y), command.ButtonID, command.Pressed);
            m_FpsCamera->OnMouseButon(float(command.X), float(command.Y), command.ButtonID, command.Pressed);
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

void SampleGame::RecordFrame(hd::util::CommandBuffer& commandBuffer)
{
    hd::gfx::TextureHandle framebuffer = m_GfxSwapchain->GetActiveFramebuffer();

    hd::gfx::GraphicCommandsStream commandStream{ m_GraphicCommands };
    commandStream.ClearRenderTarget(framebuffer, { 0.0f, 0.0f, 1.0f, 1.0f });

    m_GBufferPass->RenderFrame(m_GraphicCommands, *m_FpsCamera, *m_SceneMeshCollection);
    m_LightingPass->RenderFrame(m_GraphicCommands, *m_FpsCamera, m_GBufferPass->GetSurface0(), m_GBufferPass->GetSurface1(), m_GBufferPass->GetSurface2(), m_GBufferPass->GetDepth(),
        framebuffer);

    m_DebugOverlay->Render(framebuffer, m_GraphicCommands);
}

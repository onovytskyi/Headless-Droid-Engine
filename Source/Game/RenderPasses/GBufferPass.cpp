#include "Game/Bootstrap.h"

#include "Game/RenderPasses/GBufferPass.h"

#include "Framework/Graphics/GraphicCommands.h"
#include "Framework/Math/Math.h"

struct FrameData
{
    hd::math::Matrix4x4 World;
    hd::math::Matrix4x4 WorldInvTrans;
    hd::math::Matrix4x4 WorldViewProj;
    hd::math::Vectorf3 EyePositionWorld;
};

GBufferPass::GBufferPass(hd::gfx::Backend& backend, hd::gfx::Device& device, uint32_t width, uint32_t height)
    : m_Device{ device }
    , m_Width{ 0 }
    , m_Height{ 0 }
    , m_RenderState{ backend }
    , m_FrameConstants{}
    , m_Surface0{}
    , m_Surface1{}
    , m_Surface2{}
    , m_Depth{}
    , m_DepthClearColor{ 0.0f, 0.0f, 0.0f, 0.0f }
    , m_GBufferClearColor{ 0.0f, 0.0f, 0.0f, 0.0f }
{
    m_FrameConstants = m_Device.CreateBuffer(1, sizeof(FrameData), hd::gfx::BufferFlagsBits::ConstantBuffer);

    m_RenderState.SetVS(u8"GBuffer.hlsl", u8"MainVS");
    m_RenderState.SetPS(u8"GBuffer.hlsl", u8"MainPS");
    m_RenderState.SetRenderTargetFormat(0, hd::gfx::GraphicFormat::RGBA8UNorm);
    m_RenderState.SetRenderTargetFormat(1, hd::gfx::GraphicFormat::RGBA8UNorm);
    m_RenderState.SetRenderTargetFormat(2, hd::gfx::GraphicFormat::RGBA16Float);
    m_RenderState.SetDepthEnable(true);
    m_RenderState.SetDepthComparisonFunction(hd::gfx::ComparisonFunc::GreaterEqual);
    m_RenderState.SetStencilEnable(false);
    m_RenderState.SetDepthStencilFormat(hd::gfx::GraphicFormat::D24UNorm_S8UInt);
    m_RenderState.SetPrimitiveType(hd::gfx::PrimitiveType::Triangle);

    ResizeTextures(width, height);
}

GBufferPass::~GBufferPass()
{
    m_Device.DestroyBuffer(m_FrameConstants);
    m_Device.DestroyTexture(m_Surface0);
    m_Device.DestroyTexture(m_Surface1);
    m_Device.DestroyTexture(m_Surface2);
    m_Device.DestroyTexture(m_Depth);
}

void GBufferPass::RenderFrame(hd::util::CommandBuffer& graphicsCommands, hd::scene::FpsCamera& camera, hd::render::MeshCollection& meshes)
{
    hd::gfx::GraphicCommandsStream commandStream{ graphicsCommands };

    commandStream.ClearDepthStencil(m_Depth, m_DepthClearColor[0], uint32_t(m_DepthClearColor[1]));
    commandStream.ClearRenderTarget(m_Surface0, m_GBufferClearColor);
    commandStream.ClearRenderTarget(m_Surface1, m_GBufferClearColor);
    commandStream.ClearRenderTarget(m_Surface2, m_GBufferClearColor);

    FrameData frameData{};
    // #TODO add support for per object transform
    frameData.World = hd::math::MatrixIdentity();
    frameData.WorldInvTrans = hd::math::MatrixInverseTranspose(frameData.World);
    frameData.WorldViewProj = hd::math::MatrixTranspose(hd::math::MatrixMultiply(frameData.World, camera.GetViewProjectionMatrix()));
    frameData.EyePositionWorld = camera.GetPosition();
    commandStream.UpdateBuffer(m_FrameConstants, 0, &frameData, sizeof(frameData));
    commandStream.UseAsConstantBuffer(m_FrameConstants);

    commandStream.SetRenderState(&m_RenderState);
    commandStream.SetTopologyType(hd::gfx::TopologyType::List);
    commandStream.SetViewport({ 0.0f, float(m_Width), 0.0f, float(m_Height), 0.0f, 1.0f });
    commandStream.SetScissorRect({ 0, m_Width, 0, m_Height });
    commandStream.SetDepthStencil(m_Depth);
    hd::gfx::SetRenderTargetsCommand& command = hd::gfx::SetRenderTargetsCommand::WriteTo(graphicsCommands, 3);
    command.Targets[0] = m_Surface0;
    command.Targets[1] = m_Surface1;
    command.Targets[2] = m_Surface2;

    commandStream.SetRootVariable(0, m_Device.GetCBVShaderIndex(m_FrameConstants));
    commandStream.SetRootVariable(1, m_Device.GetSRVShaderIndex(meshes.GetMaterialsBuffer()));
    commandStream.SetRootVariable(2, m_Device.GetSRVShaderIndex(meshes.GetMeshesBuffer()));
    commandStream.SetRootVariable(3, m_Device.GetSRVShaderIndex(meshes.GetIndicesBuffer()));
    commandStream.SetRootVariable(4, m_Device.GetSRVShaderIndex(meshes.GetVerticesBuffer()));

    for (uint32_t meshIdx = 0; meshIdx < meshes.GetMeshCount(); ++meshIdx)
    {
        commandStream.SetRootVariable(5, meshIdx);
        commandStream.DrawInstanced(meshes.GetMeshIndexCount(meshIdx), 1);
    }
}

void GBufferPass::ResizeTextures(uint32_t width, uint32_t height)
{
    if (m_Width != width || m_Height != height)
    {
        m_Width = width;
        m_Height = height;

        if (m_Surface0 != hd::gfx::INVALID_TEXTURE_HANDLE)
        {
            m_Device.DestroyTexture(m_Surface0);
        }

        if (m_Surface1 != hd::gfx::INVALID_TEXTURE_HANDLE)
        {
            m_Device.DestroyTexture(m_Surface1);
        }

        if (m_Surface2 != hd::gfx::INVALID_TEXTURE_HANDLE)
        {
            m_Device.DestroyTexture(m_Surface2);
        }

        if (m_Depth != hd::gfx::INVALID_TEXTURE_HANDLE)
        {
            m_Device.DestroyTexture(m_Depth);
        }

        m_Depth = m_Device.CreateTexture(width, height, 1, 1, hd::gfx::GraphicFormat::D24UNorm_S8UInt,
            hd::gfx::TextureFlagsBits::DepthStencil | hd::gfx::TextureFlagsBits::ShaderResource, hd::gfx::TextureDimenstion::Texture2D, m_DepthClearColor.data());

        m_Surface0 = m_Device.CreateTexture(width, height, 1, 1, hd::gfx::GraphicFormat::RGBA8UNorm,
            hd::gfx::TextureFlagsBits::RenderTarget | hd::gfx::TextureFlagsBits::ShaderResource, hd::gfx::TextureDimenstion::Texture2D, m_GBufferClearColor.data());
        m_Surface1 = m_Device.CreateTexture(width, height, 1, 1, hd::gfx::GraphicFormat::RGBA8UNorm,
            hd::gfx::TextureFlagsBits::RenderTarget | hd::gfx::TextureFlagsBits::ShaderResource, hd::gfx::TextureDimenstion::Texture2D, m_GBufferClearColor.data());
        m_Surface2 = m_Device.CreateTexture(width, height, 1, 1, hd::gfx::GraphicFormat::RGBA16Float,
            hd::gfx::TextureFlagsBits::RenderTarget | hd::gfx::TextureFlagsBits::ShaderResource, hd::gfx::TextureDimenstion::Texture2D, m_GBufferClearColor.data());
    }
}

hd::gfx::TextureHandle GBufferPass::GetSurface0() const
{
    return m_Surface0;
}

hd::gfx::TextureHandle GBufferPass::GetSurface1() const
{
    return m_Surface1;
}

hd::gfx::TextureHandle GBufferPass::GetSurface2() const
{
    return m_Surface2;
}

hd::gfx::TextureHandle GBufferPass::GetDepth() const
{
    return m_Depth;
}

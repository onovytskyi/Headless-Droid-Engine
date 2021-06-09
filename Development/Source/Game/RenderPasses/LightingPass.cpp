#include "Game/Bootstrap.h"

#include "Game/RenderPasses/LightingPass.h"

#include "Engine/Framework/Graphics/GraphicCommands.h"
#include "Engine/Framework/Math/Math.h"

struct FrameData
{
    hd::math::Matrix4x4 ProjInv;
    hd::math::Matrix4x4 ViewInv;
    hd::math::Vectorf3 EyePositionWorld;
    float SunIntensity;
    hd::math::Vectorf3 SunDirection;
};

LightingPass::LightingPass(hd::gfx::Backend& backend, hd::gfx::Device& device, hd::gfx::GraphicFormat targetFormat)
    : m_Device{ device }
    , m_RenderState{ backend }
    , m_FrameConstants{}
{
    m_FrameConstants = m_Device.CreateBuffer(1, sizeof(FrameData), hd::gfx::BufferFlagsBits::ConstantBuffer);

    m_RenderState.SetVS(u8"FullscreenTriangle.hlsl", u8"MainVS");
    m_RenderState.SetPS(u8"Lighting.hlsl", u8"MainPS");
    m_RenderState.SetRenderTargetFormat(0, targetFormat);
    m_RenderState.SetDepthEnable(false);
    m_RenderState.SetStencilEnable(false);
    m_RenderState.SetBlendType(hd::gfx::BlendType::Alpha, hd::gfx::BlendType::None);
    m_RenderState.SetPrimitiveType(hd::gfx::PrimitiveType::Triangle);
}

LightingPass::~LightingPass()
{
    m_Device.DestroyBuffer(m_FrameConstants);
}

void barbar(unsigned int) {}

void LightingPass::RenderFrame(hd::util::CommandBuffer& graphicsCommands, hd::scene::FpsCamera& camera, hd::gfx::TextureHandle surface0, hd::gfx::TextureHandle surface1,
    hd::gfx::TextureHandle surface2, hd::gfx::TextureHandle depth, hd::gfx::TextureHandle target)
{
    hd::gfx::GraphicCommandsStream commandStream{ graphicsCommands };

    FrameData frameData{};

    frameData.ProjInv = hd::math::MatrixInverseTranspose(camera.GetProjectionMatrix());
    frameData.ViewInv = hd::math::MatrixInverseTranspose(camera.GetViewMatrix());
    frameData.EyePositionWorld = camera.GetPosition();
    // #TODO Make SunDirection and SunIntencity configurable
    frameData.SunDirection = { -1.0f, -1.0f, -1.0f };
    frameData.SunIntensity = 4.0f;
    commandStream.UpdateBuffer(m_FrameConstants, 0, &frameData, sizeof(frameData));
    commandStream.UseAsConstantBuffer(m_FrameConstants);

    uint64_t width{};
    uint32_t height{};
    m_Device.GetTextureDimensions(target, width, height);

    commandStream.SetRenderState(&m_RenderState);
    commandStream.SetTopologyType(hd::gfx::TopologyType::List);
    commandStream.SetViewport({ 0.0f, float(width), 0.0f, float(height), 0.0f, 1.0f });
    commandStream.SetScissorRect({ 0, uint32_t(width), 0, height });
    commandStream.SetRenderTarget(target);

    commandStream.UseAsReadableResource(surface0);
    commandStream.UseAsReadableResource(surface1);
    commandStream.UseAsReadableResource(surface2);
    commandStream.UseAsReadableResource(depth);

    commandStream.SetRootVariable(0, m_Device.GetCBVShaderIndex(m_FrameConstants));
    commandStream.SetRootVariable(1, m_Device.GetSRVShaderIndex(surface0));
    commandStream.SetRootVariable(2, m_Device.GetSRVShaderIndex(surface1));
    commandStream.SetRootVariable(3, m_Device.GetSRVShaderIndex(surface2));
    commandStream.SetRootVariable(4, m_Device.GetSRVShaderIndex(depth));

    commandStream.DrawInstanced(3, 1);
}
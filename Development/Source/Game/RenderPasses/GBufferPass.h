#pragma once

#include "Engine/Engine/Render/MeshCollection.h"
#include "Engine/Engine/Scene/FpsCamera.h"
#include "Engine/Framework/Graphics/Backend.h"
#include "Engine/Framework/Graphics/Device.h"
#include "Engine/Framework/Graphics/GraphicsTypes.h"
#include "Engine/Framework/Graphics/RenderState.h"
#include "Engine/Framework/Utils/CommandBuffer.h"

class GBufferPass
{
public:
    GBufferPass(hd::gfx::Backend& backend, hd::gfx::Device& device, uint32_t width, uint32_t height);
    ~GBufferPass();

    void RenderFrame(hd::util::CommandBuffer& graphicsCommands, hd::scene::FpsCamera& camera, hd::render::MeshCollection& meshes);
    void ResizeTextures(uint32_t width, uint32_t height);

    hd::gfx::TextureHandle GetSurface0() const;
    hd::gfx::TextureHandle GetSurface1() const;
    hd::gfx::TextureHandle GetSurface2() const;
    hd::gfx::TextureHandle GetDepth() const;

private:
    hd::gfx::Device& m_Device;

    uint32_t m_Width;
    uint32_t m_Height;

    hd::gfx::RenderState m_RenderState;
    hd::gfx::BufferHandle m_FrameConstants;
    hd::gfx::TextureHandle m_Surface0;
    hd::gfx::TextureHandle m_Surface1;
    hd::gfx::TextureHandle m_Surface2;
    hd::gfx::TextureHandle m_Depth;

    std::array<float, 4> m_DepthClearColor;
    std::array<float, 4> m_GBufferClearColor;
};
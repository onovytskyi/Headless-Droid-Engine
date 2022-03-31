#pragma once

#include "Engine/Render/MeshCollection.h"
#include "Engine/Scene/FpsCamera.h"
#include "Framework/Graphics/Backend.h"
#include "Framework/Graphics/Device.h"
#include "Framework/Graphics/GraphicsTypes.h"
#include "Framework/Graphics/RenderState.h"
#include "Framework/Utils/CommandBuffer.h"

class GBufferPass
{
public:
    GBufferPass(hd::Backend& backend, hd::Device& device, uint32_t width, uint32_t height);
    ~GBufferPass();

    void RenderFrame(hd::CommandBuffer& graphicsCommands, hd::FpsCamera& camera, hd::MeshCollection& meshes);
    void ResizeTextures(uint32_t width, uint32_t height);

    hd::TextureHandle GetSurface0() const;
    hd::TextureHandle GetSurface1() const;
    hd::TextureHandle GetSurface2() const;
    hd::TextureHandle GetDepth() const;

private:
    hd::Device& m_Device;

    uint32_t m_Width;
    uint32_t m_Height;

    hd::RenderState m_RenderState;
    hd::BufferHandle m_FrameConstants;
    hd::TextureHandle m_Surface0;
    hd::TextureHandle m_Surface1;
    hd::TextureHandle m_Surface2;
    hd::TextureHandle m_Depth;

    std::array<float, 4> m_DepthClearColor;
    std::array<float, 4> m_GBufferClearColor;
};
#pragma once

#include "Engine/Render/MeshCollection.h"
#include "Engine/Scene/FpsCamera.h"
#include "Framework/Graphics/Backend.h"
#include "Framework/Graphics/Device.h"
#include "Framework/Graphics/GraphicsTypes.h"
#include "Framework/Graphics/RenderState.h"
#include "Framework/Utils/CommandBuffer.h"

class LightingPass
{
public:
    LightingPass(hd::gfx::Backend& backend, hd::gfx::Device& device, hd::gfx::GraphicFormat targetFormat);
    ~LightingPass();

    void RenderFrame(hd::util::CommandBuffer& graphicsCommands, hd::scene::FpsCamera& camera, hd::gfx::TextureHandle surface0, hd::gfx::TextureHandle surface1, 
        hd::gfx::TextureHandle surface2, hd::gfx::TextureHandle depth, hd::gfx::TextureHandle target);

private:
    hd::gfx::Device& m_Device;

    hd::gfx::RenderState m_RenderState;
    hd::gfx::BufferHandle m_FrameConstants;
};
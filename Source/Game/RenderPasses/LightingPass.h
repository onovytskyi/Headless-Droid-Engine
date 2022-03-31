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
    LightingPass(hd::Backend& backend, hd::Device& device, hd::GraphicFormat targetFormat);
    ~LightingPass();

    void RenderFrame(hd::CommandBuffer& graphicsCommands, hd::FpsCamera& camera, hd::TextureHandle surface0, hd::TextureHandle surface1, 
        hd::TextureHandle surface2, hd::TextureHandle depth, hd::TextureHandle target);

private:
    hd::Device& m_Device;

    hd::RenderState m_RenderState;
    hd::BufferHandle m_FrameConstants;
};
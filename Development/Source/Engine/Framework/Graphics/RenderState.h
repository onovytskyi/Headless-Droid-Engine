#pragma once

#include "Engine/Framework/Graphics/DX12/RenderStateDX12.h"

namespace hd
{
    namespace gfx
    {
        class Backend;

        class RenderState : public RenderStatePlatform
        {
        public:
            RenderState(Backend& backend);
            ~RenderState();

            hdNoncopyable(RenderState)

            void SetVS(char8_t const* shaderName, char8_t const* entryPoint);
            void SetPS(char8_t const* shaderName, char8_t const* entryPoint);
            void SetCS(char8_t const* shaderName, char8_t const* entryPoint);
            void SetRenderTargetFormat(uint32_t index, GraphicFormat format);
            void SetDepthEnable(bool value);
            void SetStencilEnable(bool value);
            void SetBlendEnable(bool value);
            void SetBlendEnable(uint32_t index, bool value);
            void SetPrimitiveType(PrimitiveType primitiveType);
            PrimitiveType GetPrimitiveType() const;
        };
    }
}
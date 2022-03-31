#pragma once

#include "Framework/Graphics/DX12/RenderStateDX12.h"

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
            void SetDepthStencilFormat(GraphicFormat format);
            void SetDepthEnable(bool value);
            void SetDepthComparisonFunction(ComparisonFunc comparisonFunc);
            void SetStencilEnable(bool value);
            void SetBlendType(BlendType color, BlendType alpha);
            void SetBlendType(uint32_t index, BlendType color, BlendType alpha);
            void SetPrimitiveType(PrimitiveType primitiveType);
            PrimitiveType GetPrimitiveType() const;
        };
    }
}
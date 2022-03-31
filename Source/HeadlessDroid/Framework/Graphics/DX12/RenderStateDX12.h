#pragma once

#if defined(HD_GRAPHICS_API_DX12)

#include "Framework/Graphics/GraphicsTypes.h"
#include "Framework/Utils/CommandBuffer.h"

namespace hd
{
    namespace gfx
    {
        class Backend;
        class Device;

        class RenderStatePlatform
        {
        public:
            RenderStatePlatform(Backend& backend);
            ~RenderStatePlatform();

            void Compile(Device& device);

            ID3D12PipelineState* GetNativePipelineState() const;

#if defined(HD_ENABLE_RESOURCE_COOKING)
            void Rebuild(bool ignoreCache);
#endif

        protected:
            void CreatePipelineState(Device& device);
            void SetupDefaults(D3D12_DEPTH_STENCIL_DESC& desc);
            void SetupDefaults(D3D12_BLEND_DESC& desc);

            Backend& m_Backend;
            ComPtr<ID3D12PipelineState> m_PipelineState;
            util::CommandBuffer m_PipelineStream;

            D3D12_SHADER_BYTECODE* m_VS;
            D3D12_SHADER_BYTECODE* m_PS;
            D3D12_SHADER_BYTECODE* m_CS;
            D3D12_RT_FORMAT_ARRAY* m_RTFormats;
            DXGI_FORMAT* m_DSFormat;
            D3D12_DEPTH_STENCIL_DESC* m_DepthStencil;
            D3D12_BLEND_DESC* m_Blend;
            D3D12_PRIMITIVE_TOPOLOGY_TYPE* m_PrimitiveTopology;

            PrimitiveType m_PrimitiveType;

#if defined(HD_ENABLE_RESOURCE_COOKING)
            Device* m_Device;
            char8_t const* m_VSShaderName;
            char8_t const* m_VSEntryPoint;
            char8_t const* m_PSShaderName;
            char8_t const* m_PSEntryPoint;
            char8_t const* m_CSShaderName;
            char8_t const* m_CSEntryPoint;
#endif
        };
    }
}

#endif
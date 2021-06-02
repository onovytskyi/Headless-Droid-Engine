#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/RenderState.h"

#if defined(HD_GRAPHICS_API_DX12)

#include "Engine/Debug/Assert.h"
#include "Engine/Foundation/Memory/Utils.h"
#include "Engine/Framework/Graphics/Backend.h"
#include "Engine/Framework/Graphics/DX12/ShaderManagerDX12.h"
#include "Engine/Framework/Graphics/DX12/UtilsDX12.h"
#include "Engine/Framework/Graphics/Device.h"

namespace hd
{
    namespace gfx
    {
        struct alignas(void*) StreamingVS
        {
        private:
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS;
        public:
            D3D12_SHADER_BYTECODE ShaderBytecode;
        };

        struct alignas(void*) StreamingPS
        {
        private:
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS;
        public:
            D3D12_SHADER_BYTECODE ShaderBytecode;
        };

        struct alignas(void*) StreamingCS
        {
        private:
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS;
        public:
            D3D12_SHADER_BYTECODE ShaderBytecode;
        };

        struct alignas(void*) StreamingRasterizerDesc
        {
        private:
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER;
        public:
            D3D12_RASTERIZER_DESC Desc;
        };

        struct alignas(void*) StreamingPrimitiveTopology
        {
        private:
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY;
        public:
            D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType;
        };

        struct alignas(void*) StreamingTargetFormats
        {
        private:
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS;
        public:
            D3D12_RT_FORMAT_ARRAY Formats;
        };

        struct alignas(void*) StreamingDepthStencilFormat
        {
        private:
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT;
        public:
            DXGI_FORMAT Format;
        };

        struct alignas(void*) StreamingDepthStencil
        {
        private:
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL;
        public:
            D3D12_DEPTH_STENCIL_DESC Desc;
        };

        struct alignas(void*) StreamingBlend
        {
        private:
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND;
        public:
            D3D12_BLEND_DESC Desc;
        };

        struct alignas(void*) StreamingRootSignature
        {
        private:
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE;
        public:
            ID3D12RootSignature* RootSignature;
        };

        RenderStatePlatform::RenderStatePlatform(Backend& backend)
            : m_Backend{ backend }
            , m_PipelineStream{ mem::MB(2) }
            , m_VS{}
            , m_PS{}
            , m_CS{}
            , m_RTFormats{}
            , m_DepthStencil{}
            , m_Blend{}
            , m_PrimitiveTopology{}
            , m_PrimitiveType{}
        {

        }

        RenderStatePlatform::~RenderStatePlatform()
        {

        }

        void RenderStatePlatform::Compile(Device& device)
        {
            if (m_PipelineState.Get() == nullptr)
            {
                StreamingRootSignature& rootSignature = m_PipelineStream.Write<StreamingRootSignature>();
                rootSignature = {};
                rootSignature.RootSignature = device.GetNativeRootSignature();

                D3D12_PIPELINE_STATE_STREAM_DESC pipelineDesc{};
                pipelineDesc.pPipelineStateSubobjectStream = m_PipelineStream.GetBuffer().GetData();
                pipelineDesc.SizeInBytes = m_PipelineStream.GetBuffer().GetSize();

                hdEnsure(device.GetNativeDevice()->CreatePipelineState(&pipelineDesc, IID_PPV_ARGS(m_PipelineState.GetAddressOf())));

                m_PipelineStream.Clear();
            }
        }

        ID3D12PipelineState* RenderStatePlatform::GetNativePipelineState() const
        {
            hdAssert(m_PipelineState.Get() != nullptr, u8"Render state wasn't compiled.");

            return m_PipelineState.Get();
        }

        void RenderStatePlatform::SetupDefaults(D3D12_DEPTH_STENCIL_DESC& desc)
        {
            desc = {};
            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
            desc.StencilEnable = FALSE;
            desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
            desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
            desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
            desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
            desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
            desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        }

        void RenderStatePlatform::SetupDefaults(D3D12_BLEND_DESC& desc)
        {
            desc = {};
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0] = ConstructBlendDesc(BlendType::None, BlendType::None);
        }

        void RenderState::SetVS(char8_t const* shaderName, char8_t const* entryPoint)
        {
            if (m_VS == nullptr)
            {
                StreamingVS& vs = m_PipelineStream.Write<StreamingVS>();
                vs = {};

                m_VS = &vs.ShaderBytecode;
            }

            mem::Buffer& shaderMicrocode = m_Backend.GetShaderManager().GetShader(shaderName, entryPoint, cfg::GetVSProfile());
            m_VS->pShaderBytecode = shaderMicrocode.GetData();
            m_VS->BytecodeLength = shaderMicrocode.GetSize();
        }

        void RenderState::SetPS(char8_t const* shaderName, char8_t const* entryPoint)
        {
            if (m_PS == nullptr)
            {
                StreamingPS& ps = m_PipelineStream.Write<StreamingPS>();
                ps = {};

                m_PS = &ps.ShaderBytecode;
            }

            mem::Buffer& shaderMicrocode = m_Backend.GetShaderManager().GetShader(shaderName, entryPoint, cfg::GetPSProfile());
            m_PS->pShaderBytecode = shaderMicrocode.GetData();
            m_PS->BytecodeLength = shaderMicrocode.GetSize();
        }

        void RenderState::SetCS(char8_t const* shaderName, char8_t const* entryPoint)
        {
            if (m_CS == nullptr)
            {
                StreamingCS& cs = m_PipelineStream.Write<StreamingCS>();
                cs = {};

                m_CS = &cs.ShaderBytecode;
            }

            mem::Buffer& shaderMicrocode = m_Backend.GetShaderManager().GetShader(shaderName, entryPoint, cfg::GetCSProfile());
            m_CS->pShaderBytecode = shaderMicrocode.GetData();
            m_CS->BytecodeLength = shaderMicrocode.GetSize();
        }

        void RenderState::SetRenderTargetFormat(uint32_t index, GraphicFormat format)
        {
            hdAssert(index < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, u8"Render target index is out of supported range.");

            if (m_RTFormats == nullptr)
            {
                StreamingTargetFormats& targetFormats = m_PipelineStream.Write<StreamingTargetFormats>();
                targetFormats = {};

                m_RTFormats = &targetFormats.Formats;
            }

            m_RTFormats->RTFormats[index] = ConvertToWriteableFormat(format);
            m_RTFormats->NumRenderTargets = std::max(m_RTFormats->NumRenderTargets, index + 1);
        }

        void RenderState::SetDepthStencilFormat(GraphicFormat format)
        {
            if (m_DSFormat == nullptr)
            {
                StreamingDepthStencilFormat& depthFormat = m_PipelineStream.Write<StreamingDepthStencilFormat>();
                depthFormat = {};

                m_DSFormat = &depthFormat.Format;
            }

            *m_DSFormat = ConvertToWriteableFormat(format);
        }

        void RenderState::SetDepthEnable(bool value)
        {
            if (m_DepthStencil == nullptr)
            {
                StreamingDepthStencil& depthStencil = m_PipelineStream.Write<StreamingDepthStencil>();
                depthStencil = {};
                SetupDefaults(depthStencil.Desc);

                m_DepthStencil = &depthStencil.Desc;
            }

            m_DepthStencil->DepthEnable = value ? TRUE : FALSE;
        }

        void RenderState::SetDepthComparisonFunction(ComparisonFunc comparisonFunc)
        {
            if (m_DepthStencil == nullptr)
            {
                StreamingDepthStencil& depthStencil = m_PipelineStream.Write<StreamingDepthStencil>();
                depthStencil = {};
                SetupDefaults(depthStencil.Desc);

                m_DepthStencil = &depthStencil.Desc;
            }

            m_DepthStencil->DepthFunc = ConvertToComparisonFunc(comparisonFunc);
        }

        void RenderState::SetStencilEnable(bool value)
        {
            if (m_DepthStencil == nullptr)
            {
                StreamingDepthStencil& depthStencil = m_PipelineStream.Write<StreamingDepthStencil>();
                depthStencil = {};
                SetupDefaults(depthStencil.Desc);

                m_DepthStencil = &depthStencil.Desc;
            }

            m_DepthStencil->StencilEnable = value ? TRUE : FALSE;
        }

        void RenderState::SetBlendType(BlendType color, BlendType alpha)
        {
            if (m_Blend == nullptr)
            {
                StreamingBlend& blend = m_PipelineStream.Write<StreamingBlend>();
                blend = {};
                SetupDefaults(blend.Desc);

                m_Blend = &blend.Desc;
            }

            m_Blend->IndependentBlendEnable = FALSE;
            m_Blend->RenderTarget[0] = ConstructBlendDesc(color, alpha);
        }

        void RenderState::SetBlendType(uint32_t index, BlendType color, BlendType alpha)
        {
            hdAssert(index < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT, u8"Render target index is out of supported range.");

            if (m_Blend == nullptr)
            {
                StreamingBlend& blend = m_PipelineStream.Write<StreamingBlend>();
                blend = {};
                SetupDefaults(blend.Desc);

                m_Blend = &blend.Desc;
            }

            m_Blend->IndependentBlendEnable = TRUE;
            m_Blend->RenderTarget[index] = ConstructBlendDesc(color, alpha);
        }

        void RenderState::SetPrimitiveType(PrimitiveType primitiveType)
        {
            if (m_PrimitiveTopology == nullptr)
            {
                StreamingPrimitiveTopology& primitiveTopology = m_PipelineStream.Write<StreamingPrimitiveTopology>();
                primitiveTopology = {};
                primitiveTopology.TopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;

                m_PrimitiveTopology = &primitiveTopology.TopologyType;
            }

            *m_PrimitiveTopology = ConvertToTopologyType(primitiveType);
            m_PrimitiveType = primitiveType;
        }

        PrimitiveType RenderState::GetPrimitiveType() const
        {
            return m_PrimitiveType;
        }
    }
}

#endif
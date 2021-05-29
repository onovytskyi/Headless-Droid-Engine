#pragma once

#include "Engine/Framework/Graphics/GraphicsTypes.h"

namespace hd
{
    namespace util
    {
        class CommandBuffer;
        class CommandBufferReader;
    }

    namespace gfx
    {
        class RenderState;

        enum class GraphicCommandType : uint32_t
        {
            ClearRenderTarget,
            UpdateBuffer,
            UpdateTexture,
            SetRenderState,
            SetTopologyType,
            DrawInstanced,
            SetViewports,
            SetScissorRects,
            SetRenderTargets,
            SetRootVariable
        };

        struct ClearRenderTargetCommand
        {
            static ClearRenderTargetCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static ClearRenderTargetCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            TextureHandle Target;
            std::array<float, 4> Color;
        };

        struct UpdateTextureCommand
        {
            static UpdateTextureCommand& WriteTo(util::CommandBuffer& commandBuffer, size_t dataSize);
            static UpdateTextureCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            TextureHandle Target;
            uint32_t FirstSubresource;
            uint32_t NumSubresources;
            std::byte* Data;
            size_t Size;
        };

        struct UpdateBufferCommand
        {
            static UpdateBufferCommand& WriteTo(util::CommandBuffer& commandBuffer, size_t dataSize);
            static UpdateBufferCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            BufferHandle Target;
            size_t Offset;
            std::byte* Data;
            size_t Size;
        };

        struct SetRenderStateCommand
        {
            static SetRenderStateCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static SetRenderStateCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            RenderState* State;
        };

        struct SetTopologyTypeCommand
        {
            static SetTopologyTypeCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static SetTopologyTypeCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            TopologyType Type;
        };

        struct DrawInstancedCommand
        {
            static DrawInstancedCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static DrawInstancedCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            uint32_t VertexCount;
            uint32_t InstanceCount;
        };

        struct SetViewportsCommand
        {
            static SetViewportsCommand& WriteTo(util::CommandBuffer& commandBuffer, uint32_t count);
            static SetViewportsCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            Viewport* Viewports;
            uint32_t Count;
        };

        struct SetScissorRectsCommand
        {
            static SetScissorRectsCommand& WriteTo(util::CommandBuffer& commandBuffer, uint32_t count);
            static SetScissorRectsCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            Rect* Rects;
            uint32_t Count;
        };

        struct SetRenderTargetsCommand
        {
            static SetRenderTargetsCommand& WriteTo(util::CommandBuffer& commandBuffer, uint32_t count);
            static SetRenderTargetsCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            TextureHandle* Targets;
            uint32_t Count;
        };

        struct SetRootVariableCommand
        {
            static SetRootVariableCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static SetRootVariableCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            uint32_t Index;
            uint32_t Value;
        };

        class GraphicCommandsStream
        {
        public:
            GraphicCommandsStream(util::CommandBuffer& targetBuffer);

            void ClearRenderTarget(TextureHandle target, std::array<float, 4> color);
            void UpdateBuffer(BufferHandle target, size_t offset, void* data, size_t size);
            void UpdateTexture(TextureHandle target, uint32_t firstSubresource, uint32_t numSubresources, void* data, size_t size);
            void SetRenderState(RenderState* renderState);
            void SetTopologyType(TopologyType type);
            void DrawInstanced(uint32_t vertexCount, uint32_t indexCount);
            void SetViewport(Viewport const& viewport);
            void SetScissorRect(Rect const& rect);
            void SetRenderTarget(TextureHandle target);
            void SetRootVariable(uint32_t index, uint32_t value);

        private:
            util::CommandBuffer& m_CommandBuffer;
        };
    }
}
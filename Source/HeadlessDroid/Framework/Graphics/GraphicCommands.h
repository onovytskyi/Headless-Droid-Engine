#pragma once

#include "Framework/Graphics/GraphicsTypes.h"

namespace hd
{
	class CommandBuffer;
	class CommandBufferReader;
	class RenderState;

    enum class GraphicCommandType : uint32_t
    {
        ClearRenderTarget,
        ClearDepthStencil,
        UpdateBuffer,
        UpdateTexture,
        SetRenderState,
        SetTopologyType,
        DrawInstanced,
        SetViewports,
        SetScissorRects,
        SetRenderTargets,
        SetDepthStencil,
        SetRootVariable,
        UseAsConstantBuffer,
        UseAsReadableResource,
        UseAsWriteableResource
    };

    struct ClearRenderTargetCommand
    {
        static ClearRenderTargetCommand& WriteTo(CommandBuffer& commandBuffer);
        static ClearRenderTargetCommand& ReadFrom(CommandBufferReader& commandBuffer);

        TextureHandle Target;
        std::array<float, 4> Color;
    };

    struct ClearDepthStencilCommand
    {
        static ClearDepthStencilCommand& WriteTo(CommandBuffer& commandBuffer);
        static ClearDepthStencilCommand& ReadFrom(CommandBufferReader& commandBuffer);

        TextureHandle DepthStencil;
        float Depth;
        uint32_t Stencil;
    };

    struct UpdateTextureCommand
    {
        static UpdateTextureCommand& WriteTo(CommandBuffer& commandBuffer, size_t dataSize);
        static UpdateTextureCommand& ReadFrom(CommandBufferReader& commandBuffer);

        TextureHandle Target;
        uint32_t FirstSubresource;
        uint32_t NumSubresources;
        std::byte* Data;
        size_t Size;
    };

    struct UpdateBufferCommand
    {
        static UpdateBufferCommand& WriteTo(CommandBuffer& commandBuffer, size_t dataSize);
        static UpdateBufferCommand& ReadFrom(CommandBufferReader& commandBuffer);

        BufferHandle Target;
        size_t Offset;
        std::byte* Data;
        size_t Size;
    };

    struct SetRenderStateCommand
    {
        static SetRenderStateCommand& WriteTo(CommandBuffer& commandBuffer);
        static SetRenderStateCommand& ReadFrom(CommandBufferReader& commandBuffer);

        RenderState* State;
    };

    struct SetTopologyTypeCommand
    {
        static SetTopologyTypeCommand& WriteTo(CommandBuffer& commandBuffer);
        static SetTopologyTypeCommand& ReadFrom(CommandBufferReader& commandBuffer);

        TopologyType Type;
    };

    struct DrawInstancedCommand
    {
        static DrawInstancedCommand& WriteTo(CommandBuffer& commandBuffer);
        static DrawInstancedCommand& ReadFrom(CommandBufferReader& commandBuffer);

        uint32_t VertexCount;
        uint32_t InstanceCount;
    };

    struct SetViewportsCommand
    {
        static SetViewportsCommand& WriteTo(CommandBuffer& commandBuffer, uint32_t count);
        static SetViewportsCommand& ReadFrom(CommandBufferReader& commandBuffer);

        Viewport* Viewports;
        uint32_t Count;
    };

    struct SetScissorRectsCommand
    {
        static SetScissorRectsCommand& WriteTo(CommandBuffer& commandBuffer, uint32_t count);
        static SetScissorRectsCommand& ReadFrom(CommandBufferReader& commandBuffer);

        Rect* Rects;
        uint32_t Count;
    };

    struct SetRenderTargetsCommand
    {
        static SetRenderTargetsCommand& WriteTo(CommandBuffer& commandBuffer, uint32_t count);
        static SetRenderTargetsCommand& ReadFrom(CommandBufferReader& commandBuffer);

        TextureHandle* Targets;
        uint32_t Count;
    };

    struct SetDepthStencilCommand
    {
        static SetDepthStencilCommand& WriteTo(CommandBuffer& commandBuffer);
        static SetDepthStencilCommand& ReadFrom(CommandBufferReader& commandBuffer);

        TextureHandle DepthStencil;
    };

    struct SetRootVariableCommand
    {
        static SetRootVariableCommand& WriteTo(CommandBuffer& commandBuffer);
        static SetRootVariableCommand& ReadFrom(CommandBufferReader& commandBuffer);

        uint32_t Index;
        uint32_t Value;
    };

    struct UseAsConstantBufferCommand
    {
        static UseAsConstantBufferCommand& WriteTo(CommandBuffer& commandBuffer);
        static UseAsConstantBufferCommand& ReadFrom(CommandBufferReader& commandBuffer);

        BufferHandle Buffer;
    };

    struct UseAsReadableResourceCommand
    {
        static UseAsReadableResourceCommand& WriteTo(CommandBuffer& commandBuffer);
        static UseAsReadableResourceCommand& ReadFrom(CommandBufferReader& commandBuffer);

        BufferHandle Buffer;
        TextureHandle Texture;
    };

    struct UseAsWriteableResourceCommand
    {
        static UseAsWriteableResourceCommand& WriteTo(CommandBuffer& commandBuffer);
        static UseAsWriteableResourceCommand& ReadFrom(CommandBufferReader& commandBuffer);


        BufferHandle Buffer;
        TextureHandle Texture;
    };

    class GraphicCommandsStream
    {
    public:
        GraphicCommandsStream(CommandBuffer& targetBuffer);

        void ClearRenderTarget(TextureHandle target, std::array<float, 4> color);
        void ClearDepthStencil(TextureHandle depthStencil, float depth, uint32_t stencil);
        void UpdateBuffer(BufferHandle target, size_t offset, void const* data, size_t size);
        void UpdateTexture(TextureHandle target, uint32_t firstSubresource, uint32_t numSubresources, void const* data, size_t size);
        void SetRenderState(RenderState* renderState);
        void SetTopologyType(TopologyType type);
        void DrawInstanced(uint32_t vertexCount, uint32_t indexCount);
        void SetViewport(Viewport const& viewport);
        void SetScissorRect(Rect const& rect);
        void SetRenderTarget(TextureHandle target);
        void SetDepthStencil(TextureHandle depthStencil);
        void SetRootVariable(uint32_t index, uint32_t value);
        void UseAsConstantBuffer(BufferHandle buffer);
        void UseAsReadableResource(BufferHandle buffer);
        void UseAsReadableResource(TextureHandle texture);
        void UseAsWriteableResource(BufferHandle buffer);
        void UseAsWriteableResource(TextureHandle texture);

    private:
        CommandBuffer& m_CommandBuffer;
    };
}
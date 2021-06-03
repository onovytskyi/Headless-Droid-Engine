#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/GraphicCommands.h"

#include "Engine/Framework/Utils/CommandBuffer.h"
#include "Engine/Framework/Utils/CommandBufferReader.h"

namespace hd
{
    namespace gfx
    {
        template<typename Command>
        Command& WriteGraphicCommand(util::CommandBuffer& commandBuffer, GraphicCommandType commandType)
        {
            GraphicCommandType& commandID = commandBuffer.Write<GraphicCommandType>();
            commandID = commandType;

            return commandBuffer.Write<Command>();
        }

        ClearRenderTargetCommand& ClearRenderTargetCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<ClearRenderTargetCommand>(commandBuffer, GraphicCommandType::ClearRenderTarget);
        }

        ClearRenderTargetCommand& ClearRenderTargetCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<ClearRenderTargetCommand>();
        }

        ClearDepthStencilCommand& ClearDepthStencilCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<ClearDepthStencilCommand>(commandBuffer, GraphicCommandType::ClearDepthStencil);
        }

        ClearDepthStencilCommand& ClearDepthStencilCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<ClearDepthStencilCommand>();
        }

        UpdateTextureCommand& UpdateTextureCommand::WriteTo(util::CommandBuffer& commandBuffer, size_t dataSize)
        {
            UpdateTextureCommand& command = WriteGraphicCommand<UpdateTextureCommand>(commandBuffer, GraphicCommandType::UpdateTexture);
            command.Data = &commandBuffer.Write<std::byte>(dataSize);
            command.Size = dataSize;

            return command;
        }

        UpdateTextureCommand& UpdateTextureCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            UpdateTextureCommand& command = commandBuffer.Read<UpdateTextureCommand>();
            commandBuffer.Read<std::byte>(command.Size);

            return command;
        }

        UpdateBufferCommand& UpdateBufferCommand::WriteTo(util::CommandBuffer& commandBuffer, size_t dataSize)
        {
            UpdateBufferCommand& command = WriteGraphicCommand<UpdateBufferCommand>(commandBuffer, GraphicCommandType::UpdateBuffer);
            command.Data = &commandBuffer.Write<std::byte>(dataSize);
            command.Size = dataSize;

            return command;
        }

        UpdateBufferCommand& UpdateBufferCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            UpdateBufferCommand& command = commandBuffer.Read<UpdateBufferCommand>();
            commandBuffer.Read<std::byte>(command.Size);

            return command;
        }

        SetRenderStateCommand& SetRenderStateCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<SetRenderStateCommand>(commandBuffer, GraphicCommandType::SetRenderState);
        }

        SetRenderStateCommand& SetRenderStateCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<SetRenderStateCommand>();
        }

        SetTopologyTypeCommand& SetTopologyTypeCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<SetTopologyTypeCommand>(commandBuffer, GraphicCommandType::SetTopologyType);
        }

        SetTopologyTypeCommand& SetTopologyTypeCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<SetTopologyTypeCommand>();
        }

        DrawInstancedCommand& DrawInstancedCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<DrawInstancedCommand>(commandBuffer, GraphicCommandType::DrawInstanced);
        }

        DrawInstancedCommand& DrawInstancedCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<DrawInstancedCommand>();
        }

        SetViewportsCommand& SetViewportsCommand::WriteTo(util::CommandBuffer& commandBuffer, uint32_t count)
        {
            SetViewportsCommand& command = WriteGraphicCommand<SetViewportsCommand>(commandBuffer, GraphicCommandType::SetViewports);
            command.Viewports = &commandBuffer.Write<Viewport>(count);
            command.Count = count;

            return command;
        }

        SetViewportsCommand& SetViewportsCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            SetViewportsCommand& command = commandBuffer.Read<SetViewportsCommand>();
            commandBuffer.Read<Viewport>(command.Count);

            return command;
        }

        SetScissorRectsCommand& SetScissorRectsCommand::WriteTo(util::CommandBuffer& commandBuffer, uint32_t count)
        {
            SetScissorRectsCommand& command = WriteGraphicCommand<SetScissorRectsCommand>(commandBuffer, GraphicCommandType::SetScissorRects);
            command.Rects = &commandBuffer.Write<Rect>(count);
            command.Count = count;

            return command;
        }

        SetScissorRectsCommand& SetScissorRectsCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            SetScissorRectsCommand& command = commandBuffer.Read<SetScissorRectsCommand>();
            commandBuffer.Read<Rect>(command.Count);

            return command;
        }

        SetRenderTargetsCommand& SetRenderTargetsCommand::WriteTo(util::CommandBuffer& commandBuffer, uint32_t count)
        {
            SetRenderTargetsCommand& command = WriteGraphicCommand<SetRenderTargetsCommand>(commandBuffer, GraphicCommandType::SetRenderTargets);
            command.Targets = &commandBuffer.Write<TextureHandle>(count);
            command.Count = count;

            return command;
        }

        SetRenderTargetsCommand& SetRenderTargetsCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            SetRenderTargetsCommand& command = commandBuffer.Read<SetRenderTargetsCommand>();
            commandBuffer.Read<TextureHandle>(command.Count);

            return command;
        }

        SetDepthStencilCommand& SetDepthStencilCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<SetDepthStencilCommand>(commandBuffer, GraphicCommandType::SetDepthStencil);
        }

        SetDepthStencilCommand& SetDepthStencilCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<SetDepthStencilCommand>();
        }

        SetRootVariableCommand& SetRootVariableCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<SetRootVariableCommand>(commandBuffer, GraphicCommandType::SetRootVariable);
        }

        SetRootVariableCommand& SetRootVariableCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<SetRootVariableCommand>();
        }

        UseAsConstantBufferCommand& UseAsConstantBufferCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<UseAsConstantBufferCommand>(commandBuffer, GraphicCommandType::UseAsConstantBuffer);
        }

        UseAsConstantBufferCommand& UseAsConstantBufferCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<UseAsConstantBufferCommand>();
        }

        UseAsReadableResourceCommand& UseAsReadableResourceCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<UseAsReadableResourceCommand>(commandBuffer, GraphicCommandType::UseAsReadableResource);
        }

        UseAsReadableResourceCommand& UseAsReadableResourceCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<UseAsReadableResourceCommand>();
        }

        UseAsWriteableResourceCommand& UseAsWriteableResourceCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return WriteGraphicCommand<UseAsWriteableResourceCommand>(commandBuffer, GraphicCommandType::UseAsWriteableResource);
        }

        UseAsWriteableResourceCommand& UseAsWriteableResourceCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<UseAsWriteableResourceCommand>();
        }

        GraphicCommandsStream::GraphicCommandsStream(util::CommandBuffer& targetBuffer)
            : m_CommandBuffer{ targetBuffer }
        {
        }

        void GraphicCommandsStream::ClearRenderTarget(TextureHandle target, std::array<float, 4> color)
        {
            hd::gfx::ClearRenderTargetCommand& command = hd::gfx::ClearRenderTargetCommand::WriteTo(m_CommandBuffer);
            command.Target = target;
            command.Color = color;
        }

        void GraphicCommandsStream::ClearDepthStencil(TextureHandle depthStencil, float depth, uint32_t stencil)
        {
            hd::gfx::ClearDepthStencilCommand& command = hd::gfx::ClearDepthStencilCommand::WriteTo(m_CommandBuffer);
            command.DepthStencil = depthStencil;
            command.Depth = depth;
            command.Stencil = stencil;
        }

        void GraphicCommandsStream::UpdateBuffer(BufferHandle target, size_t offset, void* data, size_t size)
        {
            gfx::UpdateBufferCommand& command = gfx::UpdateBufferCommand::WriteTo(m_CommandBuffer, size);
            command.Target = target;
            command.Offset = offset;
            command.Size = size;
            memcpy_s(command.Data, command.Size, data, command.Size);
        }

        void GraphicCommandsStream::UpdateTexture(TextureHandle target, uint32_t firstSubresource, uint32_t numSubresources, void* data, size_t size)
        {
            gfx::UpdateTextureCommand& command = gfx::UpdateTextureCommand::WriteTo(m_CommandBuffer, size);
            command.Target = target;
            command.FirstSubresource = 0;
            command.NumSubresources = gfx::ALL_SUBRESOURCES;
            command.Size = size;
            memcpy_s(command.Data, command.Size, data, command.Size);
        }

        void GraphicCommandsStream::SetRenderState(RenderState* renderState)
        {
            gfx::SetRenderStateCommand& command = gfx::SetRenderStateCommand::WriteTo(m_CommandBuffer);
            command.State = renderState;
        }

        void GraphicCommandsStream::SetTopologyType(TopologyType type)
        {
            gfx::SetTopologyTypeCommand& command = gfx::SetTopologyTypeCommand::WriteTo(m_CommandBuffer);
            command.Type = type;
        }

        void GraphicCommandsStream::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount)
        {
            gfx::DrawInstancedCommand& command = gfx::DrawInstancedCommand::WriteTo(m_CommandBuffer);
            command.VertexCount = vertexCount;
            command.InstanceCount = instanceCount;
        }

        void GraphicCommandsStream::SetViewport(Viewport const& viewport)
        {
            gfx::SetViewportsCommand& command = gfx::SetViewportsCommand::WriteTo(m_CommandBuffer, 1);
            command.Viewports[0] = viewport;
        }

        void GraphicCommandsStream::SetScissorRect(Rect const& rect)
        {
            gfx::SetScissorRectsCommand& command = gfx::SetScissorRectsCommand::WriteTo(m_CommandBuffer, 1);
            command.Rects[0] = rect;
        }

        void GraphicCommandsStream::SetRenderTarget(TextureHandle target)
        {
            gfx::SetRenderTargetsCommand& command = gfx::SetRenderTargetsCommand::WriteTo(m_CommandBuffer, 1);
            command.Targets[0] = target;
        }

        void GraphicCommandsStream::SetDepthStencil(TextureHandle depthStencil)
        {
            gfx::SetDepthStencilCommand& command = gfx::SetDepthStencilCommand::WriteTo(m_CommandBuffer);
            command.DepthStencil = depthStencil;
        }

        void GraphicCommandsStream::SetRootVariable(uint32_t index, uint32_t value)
        {
            gfx::SetRootVariableCommand& command = gfx::SetRootVariableCommand::WriteTo(m_CommandBuffer);
            command.Index = index;
            command.Value = value;
        }

        void GraphicCommandsStream::UseAsConstantBuffer(BufferHandle buffer)
        {
            gfx::UseAsConstantBufferCommand& command = gfx::UseAsConstantBufferCommand::WriteTo(m_CommandBuffer);
            command.Buffer = buffer;
        }

        void GraphicCommandsStream::UseAsReadableResource(BufferHandle buffer)
        {
            gfx::UseAsReadableResourceCommand& command = gfx::UseAsReadableResourceCommand::WriteTo(m_CommandBuffer);
            command.Buffer = buffer;
            command.Texture = INVALID_TEXTURE_HANDLE;
        }

        void GraphicCommandsStream::UseAsReadableResource(TextureHandle texture)
        {
            gfx::UseAsReadableResourceCommand& command = gfx::UseAsReadableResourceCommand::WriteTo(m_CommandBuffer);
            command.Buffer = INVALID_BUFFER_HANDLE;
            command.Texture = texture;
        }

        void GraphicCommandsStream::UseAsWriteableResource(BufferHandle buffer)
        {
            gfx::UseAsWriteableResourceCommand& command = gfx::UseAsWriteableResourceCommand::WriteTo(m_CommandBuffer);
            command.Buffer = buffer;
            command.Texture = INVALID_TEXTURE_HANDLE;
        }

        void GraphicCommandsStream::UseAsWriteableResource(TextureHandle texture)
        {
            gfx::UseAsWriteableResourceCommand& command = gfx::UseAsWriteableResourceCommand::WriteTo(m_CommandBuffer);
            command.Buffer = INVALID_BUFFER_HANDLE;
            command.Texture = texture;
        }
    }
}
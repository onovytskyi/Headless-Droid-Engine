#include "Config/Bootstrap.h"

#include "Framework/Graphics/GraphicCommands.h"

#include "Framework/Utils/CommandBuffer.h"
#include "Framework/Utils/CommandBufferReader.h"

namespace hd
{
	template<typename Command>
	Command& WriteGraphicCommand(CommandBuffer& commandBuffer, GraphicCommandType commandType)
	{
		GraphicCommandType& commandID = commandBuffer.Write<GraphicCommandType>();
		commandID = commandType;

		return commandBuffer.Write<Command>();
	}

	ClearRenderTargetCommand& ClearRenderTargetCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteGraphicCommand<ClearRenderTargetCommand>(commandBuffer, GraphicCommandType::ClearRenderTarget);
	}

	ClearRenderTargetCommand& ClearRenderTargetCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<ClearRenderTargetCommand>();
	}

	ClearDepthStencilCommand& ClearDepthStencilCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteGraphicCommand<ClearDepthStencilCommand>(commandBuffer, GraphicCommandType::ClearDepthStencil);
	}

	ClearDepthStencilCommand& ClearDepthStencilCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<ClearDepthStencilCommand>();
	}

	UpdateTextureCommand& UpdateTextureCommand::WriteTo(CommandBuffer& commandBuffer, size_t dataSize)
	{
		UpdateTextureCommand& command = WriteGraphicCommand<UpdateTextureCommand>(commandBuffer, GraphicCommandType::UpdateTexture);
		command.Data = &commandBuffer.Write<std::byte>(dataSize);
		command.Size = dataSize;

		return command;
	}

	UpdateTextureCommand& UpdateTextureCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		UpdateTextureCommand& command = commandBuffer.Read<UpdateTextureCommand>();
		commandBuffer.Read<std::byte>(command.Size);

		return command;
	}

	UpdateBufferCommand& UpdateBufferCommand::WriteTo(CommandBuffer& commandBuffer, size_t dataSize)
	{
		UpdateBufferCommand& command = WriteGraphicCommand<UpdateBufferCommand>(commandBuffer, GraphicCommandType::UpdateBuffer);
		command.Data = &commandBuffer.Write<std::byte>(dataSize);
		command.Size = dataSize;

		return command;
	}

	UpdateBufferCommand& UpdateBufferCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		UpdateBufferCommand& command = commandBuffer.Read<UpdateBufferCommand>();
		commandBuffer.Read<std::byte>(command.Size);

		return command;
	}

	SetRenderStateCommand& SetRenderStateCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteGraphicCommand<SetRenderStateCommand>(commandBuffer, GraphicCommandType::SetRenderState);
	}

	SetRenderStateCommand& SetRenderStateCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<SetRenderStateCommand>();
	}

	SetTopologyTypeCommand& SetTopologyTypeCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteGraphicCommand<SetTopologyTypeCommand>(commandBuffer, GraphicCommandType::SetTopologyType);
	}

	SetTopologyTypeCommand& SetTopologyTypeCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<SetTopologyTypeCommand>();
	}

	DrawInstancedCommand& DrawInstancedCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteGraphicCommand<DrawInstancedCommand>(commandBuffer, GraphicCommandType::DrawInstanced);
	}

	DrawInstancedCommand& DrawInstancedCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<DrawInstancedCommand>();
	}

	SetViewportsCommand& SetViewportsCommand::WriteTo(CommandBuffer& commandBuffer, uint32_t count)
	{
		SetViewportsCommand& command = WriteGraphicCommand<SetViewportsCommand>(commandBuffer, GraphicCommandType::SetViewports);
		command.Viewports = &commandBuffer.Write<Viewport>(count);
		command.Count = count;

		return command;
	}

	SetViewportsCommand& SetViewportsCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		SetViewportsCommand& command = commandBuffer.Read<SetViewportsCommand>();
		commandBuffer.Read<Viewport>(command.Count);

		return command;
	}

	SetScissorRectsCommand& SetScissorRectsCommand::WriteTo(CommandBuffer& commandBuffer, uint32_t count)
	{
		SetScissorRectsCommand& command = WriteGraphicCommand<SetScissorRectsCommand>(commandBuffer, GraphicCommandType::SetScissorRects);
		command.Rects = &commandBuffer.Write<Rect>(count);
		command.Count = count;

		return command;
	}

	SetScissorRectsCommand& SetScissorRectsCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		SetScissorRectsCommand& command = commandBuffer.Read<SetScissorRectsCommand>();
		commandBuffer.Read<Rect>(command.Count);

		return command;
	}

	SetRenderTargetsCommand& SetRenderTargetsCommand::WriteTo(CommandBuffer& commandBuffer, uint32_t count)
	{
		SetRenderTargetsCommand& command = WriteGraphicCommand<SetRenderTargetsCommand>(commandBuffer, GraphicCommandType::SetRenderTargets);
		command.Targets = &commandBuffer.Write<TextureHandle>(count);
		command.Count = count;

		return command;
	}

	SetRenderTargetsCommand& SetRenderTargetsCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		SetRenderTargetsCommand& command = commandBuffer.Read<SetRenderTargetsCommand>();
		commandBuffer.Read<TextureHandle>(command.Count);

		return command;
	}

	SetDepthStencilCommand& SetDepthStencilCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteGraphicCommand<SetDepthStencilCommand>(commandBuffer, GraphicCommandType::SetDepthStencil);
	}

	SetDepthStencilCommand& SetDepthStencilCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<SetDepthStencilCommand>();
	}

	SetRootVariableCommand& SetRootVariableCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteGraphicCommand<SetRootVariableCommand>(commandBuffer, GraphicCommandType::SetRootVariable);
	}

	SetRootVariableCommand& SetRootVariableCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<SetRootVariableCommand>();
	}

	UseAsConstantBufferCommand& UseAsConstantBufferCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteGraphicCommand<UseAsConstantBufferCommand>(commandBuffer, GraphicCommandType::UseAsConstantBuffer);
	}

	UseAsConstantBufferCommand& UseAsConstantBufferCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<UseAsConstantBufferCommand>();
	}

	UseAsReadableResourceCommand& UseAsReadableResourceCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteGraphicCommand<UseAsReadableResourceCommand>(commandBuffer, GraphicCommandType::UseAsReadableResource);
	}

	UseAsReadableResourceCommand& UseAsReadableResourceCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<UseAsReadableResourceCommand>();
	}

	UseAsWriteableResourceCommand& UseAsWriteableResourceCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteGraphicCommand<UseAsWriteableResourceCommand>(commandBuffer, GraphicCommandType::UseAsWriteableResource);
	}

	UseAsWriteableResourceCommand& UseAsWriteableResourceCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<UseAsWriteableResourceCommand>();
	}

	GraphicCommandsStream::GraphicCommandsStream(CommandBuffer& targetBuffer)
		: m_CommandBuffer{ targetBuffer }
	{
	}

	void GraphicCommandsStream::ClearRenderTarget(TextureHandle target, std::array<float, 4> color)
	{
		ClearRenderTargetCommand& command = ClearRenderTargetCommand::WriteTo(m_CommandBuffer);
		command.Target = target;
		command.Color = color;
	}

	void GraphicCommandsStream::ClearDepthStencil(TextureHandle depthStencil, float depth, uint32_t stencil)
	{
		ClearDepthStencilCommand& command = ClearDepthStencilCommand::WriteTo(m_CommandBuffer);
		command.DepthStencil = depthStencil;
		command.Depth = depth;
		command.Stencil = stencil;
	}

	void GraphicCommandsStream::UpdateBuffer(BufferHandle target, size_t offset, void const* data, size_t size)
	{
		UpdateBufferCommand& command = UpdateBufferCommand::WriteTo(m_CommandBuffer, size);
		command.Target = target;
		command.Offset = offset;
		command.Size = size;
		memcpy_s(command.Data, command.Size, data, command.Size);
	}

	void GraphicCommandsStream::UpdateTexture(TextureHandle target, uint32_t firstSubresource, uint32_t numSubresources, void const* data, size_t size)
	{
		UpdateTextureCommand& command = UpdateTextureCommand::WriteTo(m_CommandBuffer, size);
		command.Target = target;
		command.FirstSubresource = 0;
		command.NumSubresources = ALL_SUBRESOURCES;
		command.Size = size;
		memcpy_s(command.Data, command.Size, data, command.Size);
	}

	void GraphicCommandsStream::SetRenderState(RenderState* renderState)
	{
		SetRenderStateCommand& command = SetRenderStateCommand::WriteTo(m_CommandBuffer);
		command.State = renderState;
	}

	void GraphicCommandsStream::SetTopologyType(TopologyType type)
	{
		SetTopologyTypeCommand& command = SetTopologyTypeCommand::WriteTo(m_CommandBuffer);
		command.Type = type;
	}

	void GraphicCommandsStream::DrawInstanced(uint32_t vertexCount, uint32_t instanceCount)
	{
		DrawInstancedCommand& command = DrawInstancedCommand::WriteTo(m_CommandBuffer);
		command.VertexCount = vertexCount;
		command.InstanceCount = instanceCount;
	}

	void GraphicCommandsStream::SetViewport(Viewport const& viewport)
	{
		SetViewportsCommand& command = SetViewportsCommand::WriteTo(m_CommandBuffer, 1);
		command.Viewports[0] = viewport;
	}

	void GraphicCommandsStream::SetScissorRect(Rect const& rect)
	{
		SetScissorRectsCommand& command = SetScissorRectsCommand::WriteTo(m_CommandBuffer, 1);
		command.Rects[0] = rect;
	}

	void GraphicCommandsStream::SetRenderTarget(TextureHandle target)
	{
		SetRenderTargetsCommand& command = SetRenderTargetsCommand::WriteTo(m_CommandBuffer, 1);
		command.Targets[0] = target;
	}

	void GraphicCommandsStream::SetDepthStencil(TextureHandle depthStencil)
	{
		SetDepthStencilCommand& command = SetDepthStencilCommand::WriteTo(m_CommandBuffer);
		command.DepthStencil = depthStencil;
	}

	void GraphicCommandsStream::SetRootVariable(uint32_t index, uint32_t value)
	{
		SetRootVariableCommand& command = SetRootVariableCommand::WriteTo(m_CommandBuffer);
		command.Index = index;
		command.Value = value;
	}

	void GraphicCommandsStream::UseAsConstantBuffer(BufferHandle buffer)
	{
		UseAsConstantBufferCommand& command = UseAsConstantBufferCommand::WriteTo(m_CommandBuffer);
		command.Buffer = buffer;
	}

	void GraphicCommandsStream::UseAsReadableResource(BufferHandle buffer)
	{
		UseAsReadableResourceCommand& command = UseAsReadableResourceCommand::WriteTo(m_CommandBuffer);
		command.Buffer = buffer;
		command.Texture = INVALID_TEXTURE_HANDLE;
	}

	void GraphicCommandsStream::UseAsReadableResource(TextureHandle texture)
	{
		UseAsReadableResourceCommand& command = UseAsReadableResourceCommand::WriteTo(m_CommandBuffer);
		command.Buffer = INVALID_BUFFER_HANDLE;
		command.Texture = texture;
	}

	void GraphicCommandsStream::UseAsWriteableResource(BufferHandle buffer)
	{
		UseAsWriteableResourceCommand& command = UseAsWriteableResourceCommand::WriteTo(m_CommandBuffer);
		command.Buffer = buffer;
		command.Texture = INVALID_TEXTURE_HANDLE;
	}

	void GraphicCommandsStream::UseAsWriteableResource(TextureHandle texture)
	{
		UseAsWriteableResourceCommand& command = UseAsWriteableResourceCommand::WriteTo(m_CommandBuffer);
		command.Buffer = INVALID_BUFFER_HANDLE;
		command.Texture = texture;
	}
}

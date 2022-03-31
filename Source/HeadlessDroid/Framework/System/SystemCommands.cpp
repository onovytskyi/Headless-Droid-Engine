#include "Config/Bootstrap.h"

#include "Framework/System/SystemCommands.h"

#include "Framework/Utils/CommandBuffer.h"
#include "Framework/Utils/CommandBufferReader.h"

namespace hd
{
	template<typename Command>
	Command& WriteSystemCommand(CommandBuffer& commandBuffer, SystemCommandType commandType)
	{
		SystemCommandType& commandID = commandBuffer.Write<SystemCommandType>();
		commandID = commandType;

		return commandBuffer.Write<Command>();
	}

	WindowActivateCommand& WindowActivateCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteSystemCommand<WindowActivateCommand>(commandBuffer, SystemCommandType::WindowActivate);
	}

	WindowActivateCommand& WindowActivateCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<WindowActivateCommand>();
	}

	WindowClosedCommand& WindowClosedCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteSystemCommand<WindowClosedCommand>(commandBuffer, SystemCommandType::WindowClosed);
	}

	WindowClosedCommand& WindowClosedCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<WindowClosedCommand>();
	}

	WindowResizeCommand& WindowResizeCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteSystemCommand<WindowResizeCommand>(commandBuffer, SystemCommandType::WindowResize);
	}

	WindowResizeCommand& WindowResizeCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<WindowResizeCommand>();
	}

	MouseButtonCommand& MouseButtonCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteSystemCommand<MouseButtonCommand>(commandBuffer, SystemCommandType::MouseButton);
	}

	MouseButtonCommand& MouseButtonCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<MouseButtonCommand>();
	}

	MouseMoveCommand& MouseMoveCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteSystemCommand<MouseMoveCommand>(commandBuffer, SystemCommandType::MouseMove);
	}

	MouseMoveCommand& MouseMoveCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<MouseMoveCommand>();
	}

	MouseWheelCommand& MouseWheelCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteSystemCommand<MouseWheelCommand>(commandBuffer, SystemCommandType::MouseWheel);
	}

	MouseWheelCommand& MouseWheelCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<MouseWheelCommand>();
	}

	KeyboardCommand& KeyboardCommand::WriteTo(CommandBuffer& commandBuffer)
	{
		return WriteSystemCommand<KeyboardCommand>(commandBuffer, SystemCommandType::Keyboard);
	}

	KeyboardCommand& KeyboardCommand::ReadFrom(CommandBufferReader& commandBuffer)
	{
		return commandBuffer.Read<KeyboardCommand>();
	}
}

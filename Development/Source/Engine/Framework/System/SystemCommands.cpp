#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/System/SystemCommands.h"

#include "Engine/Framework/Utils/CommandBuffer.h"
#include "Engine/Framework/Utils/CommandBufferReader.h"

namespace hd
{
    namespace sys
    {
        template<typename Command>
        Command& PushSystemCommand(util::CommandBuffer& commandBuffer, SystemCommandType commandType)
        {
            SystemCommandType& commandID = commandBuffer.Write<SystemCommandType>();
            commandID = commandType;

            return commandBuffer.Write<Command>();
        }

        WindowActivateCommand& WindowActivateCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<WindowActivateCommand>(commandBuffer, SystemCommandType::WindowActivate);
        }

        WindowActivateCommand& WindowActivateCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<WindowActivateCommand>();
        }

        WindowClosedCommand& WindowClosedCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<WindowClosedCommand>(commandBuffer, SystemCommandType::WindowClosed);
        }

        WindowClosedCommand& WindowClosedCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<WindowClosedCommand>();
        }

        WindowResizeCommand& WindowResizeCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<WindowResizeCommand>(commandBuffer, SystemCommandType::WindowResize);
        }

        WindowResizeCommand& WindowResizeCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<WindowResizeCommand>();
        }

        MouseButtonCommand& MouseButtonCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<MouseButtonCommand>(commandBuffer, SystemCommandType::MouseButton);
        }

        MouseButtonCommand& MouseButtonCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<MouseButtonCommand>();
        }

        MouseMoveCommand& MouseMoveCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<MouseMoveCommand>(commandBuffer, SystemCommandType::MouseMove);
        }

        MouseMoveCommand& MouseMoveCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<MouseMoveCommand>();
        }

        MouseWheelCommand& MouseWheelCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<MouseWheelCommand>(commandBuffer, SystemCommandType::MouseWheel);
        }

        MouseWheelCommand& MouseWheelCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<MouseWheelCommand>();
        }

        KeyboardCommand& KeyboardCommand::WriteTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<KeyboardCommand>(commandBuffer, SystemCommandType::Keyboard);
        }

        KeyboardCommand& KeyboardCommand::ReadFrom(util::CommandBufferReader& commandBuffer)
        {
            return commandBuffer.Read<KeyboardCommand>();
        }

    }
}
#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/System/SystemCommands.h"

#include "Engine/Framework/Utils/CommandBuffer.h"

namespace hd
{
    namespace sys
    {
        template<typename Command>
        Command& PushSystemCommand(util::CommandBuffer& commandBuffer, SystemCommandType commandType)
        {
            SystemCommandType& commandID = commandBuffer.Push<SystemCommandType>();
            commandID = commandType;

            return commandBuffer.Push<Command>();
        }

        WindowActivateCommand& WindowActivateCommand::PushTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<WindowActivateCommand>(commandBuffer, SystemCommandType::WindowActivate);
        }

        WindowActivateCommand& WindowActivateCommand::PopFrom(util::CommandBuffer& commandBuffer)
        {
            return commandBuffer.Pop<WindowActivateCommand>();
        }

        WindowClosedCommand& WindowClosedCommand::PushTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<WindowClosedCommand>(commandBuffer, SystemCommandType::WindowClosed);
        }

        WindowClosedCommand& WindowClosedCommand::PopFrom(util::CommandBuffer& commandBuffer)
        {
            return commandBuffer.Pop<WindowClosedCommand>();
        }

        WindowResizeCommand& WindowResizeCommand::PushTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<WindowResizeCommand>(commandBuffer, SystemCommandType::WindowResize);
        }

        WindowResizeCommand& WindowResizeCommand::PopFrom(util::CommandBuffer& commandBuffer)
        {
            return commandBuffer.Pop<WindowResizeCommand>();
        }

        MouseButtonCommand& MouseButtonCommand::PushTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<MouseButtonCommand>(commandBuffer, SystemCommandType::MouseButton);
        }

        MouseButtonCommand& MouseButtonCommand::PopFrom(util::CommandBuffer& commandBuffer)
        {
            return commandBuffer.Pop<MouseButtonCommand>();
        }

        MouseMoveCommand& MouseMoveCommand::PushTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<MouseMoveCommand>(commandBuffer, SystemCommandType::MouseMove);
        }

        MouseMoveCommand& MouseMoveCommand::PopFrom(util::CommandBuffer& commandBuffer)
        {
            return commandBuffer.Pop<MouseMoveCommand>();
        }

        MouseWheelCommand& MouseWheelCommand::PushTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<MouseWheelCommand>(commandBuffer, SystemCommandType::MouseWheel);
        }

        MouseWheelCommand& MouseWheelCommand::PopFrom(util::CommandBuffer& commandBuffer)
        {
            return commandBuffer.Pop<MouseWheelCommand>();
        }

        KeyboardCommand& KeyboardCommand::PushTo(util::CommandBuffer& commandBuffer)
        {
            return PushSystemCommand<KeyboardCommand>(commandBuffer, SystemCommandType::Keyboard);
        }

        KeyboardCommand& KeyboardCommand::PopFrom(util::CommandBuffer& commandBuffer)
        {
            return commandBuffer.Pop<KeyboardCommand>();
        }

    }
}
#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/System/SystemCommands.h"

#include "Engine/Framework/Utils/CommandQueue.h"

namespace hd
{
    namespace sys
    {
        template<typename Command>
        Command& PushSystemCommand(util::CommandQueue& commandQueue, SystemCommandType commandType)
        {
            SystemCommandType& commandID = commandQueue.Push<SystemCommandType>();
            commandID = commandType;

            return commandQueue.Push<Command>();
        }

        WindowActivateCommand& WindowActivateCommand::PushTo(util::CommandQueue& commandQueue)
        {
            return PushSystemCommand<WindowActivateCommand>(commandQueue, SystemCommandType::WindowActivate);
        }

        WindowActivateCommand& WindowActivateCommand::PopFrom(util::CommandQueue& commandQueue)
        {
            return commandQueue.Pop<WindowActivateCommand>();
        }

        WindowClosedCommand& WindowClosedCommand::PushTo(util::CommandQueue& commandQueue)
        {
            return PushSystemCommand<WindowClosedCommand>(commandQueue, SystemCommandType::WindowClosed);
        }

        WindowClosedCommand& WindowClosedCommand::PopFrom(util::CommandQueue& commandQueue)
        {
            return commandQueue.Pop<WindowClosedCommand>();
        }

        WindowResizeCommand& WindowResizeCommand::PushTo(util::CommandQueue& commandQueue)
        {
            return PushSystemCommand<WindowResizeCommand>(commandQueue, SystemCommandType::WindowResize);
        }

        WindowResizeCommand& WindowResizeCommand::PopFrom(util::CommandQueue& commandQueue)
        {
            return commandQueue.Pop<WindowResizeCommand>();
        }

        MouseButtonCommand& MouseButtonCommand::PushTo(util::CommandQueue& commandQueue)
        {
            return PushSystemCommand<MouseButtonCommand>(commandQueue, SystemCommandType::MouseButton);
        }

        MouseButtonCommand& MouseButtonCommand::PopFrom(util::CommandQueue& commandQueue)
        {
            return commandQueue.Pop<MouseButtonCommand>();
        }

        MouseMoveCommand& MouseMoveCommand::PushTo(util::CommandQueue& commandQueue)
        {
            return PushSystemCommand<MouseMoveCommand>(commandQueue, SystemCommandType::MouseMove);
        }

        MouseMoveCommand& MouseMoveCommand::PopFrom(util::CommandQueue& commandQueue)
        {
            return commandQueue.Pop<MouseMoveCommand>();
        }

        MouseWheelCommand& MouseWheelCommand::PushTo(util::CommandQueue& commandQueue)
        {
            return PushSystemCommand<MouseWheelCommand>(commandQueue, SystemCommandType::MouseWheel);
        }

        MouseWheelCommand& MouseWheelCommand::PopFrom(util::CommandQueue& commandQueue)
        {
            return commandQueue.Pop<MouseWheelCommand>();
        }

        KeyboardCommand& KeyboardCommand::PushTo(util::CommandQueue& commandQueue)
        {
            return PushSystemCommand<KeyboardCommand>(commandQueue, SystemCommandType::Keyboard);
        }

        KeyboardCommand& KeyboardCommand::PopFrom(util::CommandQueue& commandQueue)
        {
            return commandQueue.Pop<KeyboardCommand>();
        }

    }
}
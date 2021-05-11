#include "Game/Bootstrap.h"

#include "Engine/Debug/Log.h"
#include "Engine/Engine/Memory/EngineMemoryInterface.h"
#include "Engine/Foundation/Memory/Utils.h"
#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/System/SystemCommands.h"
#include "Engine/Framework/System/SystemWindow.h"
#include "Engine/Framework/Utils/CommandQueue.h"

int main(int argc, char* argv[])
{
    hd::mem::AllocationScope persistentScope(hd::mem::GetPersistentAllocator());

    hd::sys::SystemWindow* mainWindow = persistentScope.AllocateObject<hd::sys::SystemWindow>(u8"Droid Engine", 1280, 720);
    mainWindow->SetVisible(true);

    hd::util::CommandQueue systemCommands{ hd::mem::MB(64) };

    hdLogInfo(u8"Starting game engine loop.");

    bool isRunning = true;
    while (isRunning)
    {
        mainWindow->ProcessSystemEvents(systemCommands);

        // Events processing
        while (systemCommands.HasCommands())
        {
            hd::sys::SystemCommandType& commandType = systemCommands.Pop<hd::sys::SystemCommandType>();

            switch (commandType)
            {
            case hd::sys::SystemCommandType::WindowActivate:
            {
                hd::sys::WindowActivateCommand::PopFrom(systemCommands);
            }
                break;
            case hd::sys::SystemCommandType::WindowClosed:
            {
                hd::sys::WindowClosedCommand::PopFrom(systemCommands);
                isRunning = false;
            }
                break;
            case hd::sys::SystemCommandType::WindowResize:
            {
                hd::sys::WindowResizeCommand::PopFrom(systemCommands);
            }
                break;
            case hd::sys::SystemCommandType::MouseButton:
            {
                hd::sys::MouseButtonCommand::PopFrom(systemCommands);
            }
                break;
            case hd::sys::SystemCommandType::MouseMove:
            {
                hd::sys::MouseMoveCommand::PopFrom(systemCommands);
            }
                break;
            case hd::sys::SystemCommandType::MouseWheel:
            {
                hd::sys::MouseWheelCommand::PopFrom(systemCommands);
            }
                break;
            case hd::sys::SystemCommandType::Keyboard:
            {
                hd::sys::KeyboardCommand& command = hd::sys::KeyboardCommand::PopFrom(systemCommands);
                if (command.Pressed && command.KeyID == VK_ESCAPE)
                {
                    isRunning = false;
                }
            }
                break;
            default:
                hdAssert(false, u8"Cannot process command. Unknown command type.");
                break;
            }
        }

        systemCommands.Clear();
    }

    return 0;
}
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
            hd::sys::SystemCommandType commandType = hd::sys::SystemCommandType(systemCommands.PopCommandID());

            switch (commandType)
            {
            case hd::sys::SystemCommandType::WindowActivate:
            {
                systemCommands.PopCommand<hd::sys::WindowActivateCommand>();
            }
                break;
            case hd::sys::SystemCommandType::WindowClosed:
            {
                systemCommands.PopCommand<hd::sys::WindowClosedCommand>();
                isRunning = false;
            }
                break;
            case hd::sys::SystemCommandType::WindowResize:
            {
                systemCommands.PopCommand<hd::sys::WindowResizeCommand>();
            }
                break;
            case hd::sys::SystemCommandType::MouseButton:
            {
                systemCommands.PopCommand<hd::sys::MouseButtonCommand>();
            }
                break;
            case hd::sys::SystemCommandType::MouseMove:
            {
                systemCommands.PopCommand<hd::sys::MouseMoveCommand>();
            }
                break;
            case hd::sys::SystemCommandType::MouseWheel:
            {
                systemCommands.PopCommand<hd::sys::MouseWheelCommand>();
            }
                break;
            case hd::sys::SystemCommandType::Keyboard:
            {
                hd::sys::KeyboardCommand& command = systemCommands.PopCommand<hd::sys::KeyboardCommand>();
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
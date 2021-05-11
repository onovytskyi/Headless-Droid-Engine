#pragma once

namespace hd
{
    namespace sys
    {
        enum class SystemCommandType : uint32_t
        {
            WindowActivate,
            WindowClosed,
            WindowResize,
            MouseButton,
            MouseMove,
            MouseWheel,
            Keyboard
        };

        struct WindowActivateCommand
        {
            static const SystemCommandType ID = SystemCommandType::WindowActivate;
            bool Active;
        };

        struct WindowClosedCommand
        {
            static const SystemCommandType ID = SystemCommandType::WindowClosed;
        };

        struct WindowResizeCommand
        {
            static const SystemCommandType ID = SystemCommandType::WindowResize;

            uint32_t Width;
            uint32_t Height;
        };

        struct MouseButtonCommand
        {
            static const SystemCommandType ID = SystemCommandType::MouseButton;

            uint32_t X;
            uint32_t Y;
            uint8_t ButtonID;
            bool Pressed;
        };

        struct MouseMoveCommand
        {
            static const SystemCommandType ID = SystemCommandType::MouseMove;

            uint32_t X;
            uint32_t Y;
        };

        struct MouseWheelCommand
        {
            static const SystemCommandType ID = SystemCommandType::MouseWheel;

            uint32_t X;
            uint32_t Y;
            int32_t Offset;
        };

        struct KeyboardCommand
        {
            static const SystemCommandType ID = SystemCommandType::Keyboard;

            uint8_t KeyID;
            bool Pressed;
        };
    }
}
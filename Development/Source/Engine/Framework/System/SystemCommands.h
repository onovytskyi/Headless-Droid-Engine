#pragma once

namespace hd
{
    namespace util
    {
        class CommandQueue;
    }

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
            static WindowActivateCommand& PushTo(util::CommandQueue& commandQueue);
            static WindowActivateCommand& PopFrom(util::CommandQueue& commandQueue);

            bool Active;
        };

        struct WindowClosedCommand
        {
            static WindowClosedCommand& PushTo(util::CommandQueue& commandQueue);
            static WindowClosedCommand& PopFrom(util::CommandQueue& commandQueue);
        };

        struct WindowResizeCommand
        {
            static WindowResizeCommand& PushTo(util::CommandQueue& commandQueue);
            static WindowResizeCommand& PopFrom(util::CommandQueue& commandQueue);

            uint32_t Width;
            uint32_t Height;
        };

        struct MouseButtonCommand
        {
            static MouseButtonCommand& PushTo(util::CommandQueue& commandQueue);
            static MouseButtonCommand& PopFrom(util::CommandQueue& commandQueue);

            uint32_t X;
            uint32_t Y;
            uint8_t ButtonID;
            bool Pressed;
        };

        struct MouseMoveCommand
        {
            static MouseMoveCommand& PushTo(util::CommandQueue& commandQueue);
            static MouseMoveCommand& PopFrom(util::CommandQueue& commandQueue);

            uint32_t X;
            uint32_t Y;
        };

        struct MouseWheelCommand
        {
            static MouseWheelCommand& PushTo(util::CommandQueue& commandQueue);
            static MouseWheelCommand& PopFrom(util::CommandQueue& commandQueue);

            uint32_t X;
            uint32_t Y;
            int32_t Offset;
        };

        struct KeyboardCommand
        {
            static KeyboardCommand& PushTo(util::CommandQueue& commandQueue);
            static KeyboardCommand& PopFrom(util::CommandQueue& commandQueue);

            uint8_t KeyID;
            bool Pressed;
        };
    }
}
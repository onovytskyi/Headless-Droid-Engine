#pragma once

namespace hd
{
    namespace util
    {
        class CommandBuffer;
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
            static WindowActivateCommand& PushTo(util::CommandBuffer& commandBuffer);
            static WindowActivateCommand& PopFrom(util::CommandBuffer& commandBuffer);

            bool Active;
        };

        struct WindowClosedCommand
        {
            static WindowClosedCommand& PushTo(util::CommandBuffer& commandBuffer);
            static WindowClosedCommand& PopFrom(util::CommandBuffer& commandBuffer);
        };

        struct WindowResizeCommand
        {
            static WindowResizeCommand& PushTo(util::CommandBuffer& commandBuffer);
            static WindowResizeCommand& PopFrom(util::CommandBuffer& commandBuffer);

            uint32_t Width;
            uint32_t Height;
        };

        struct MouseButtonCommand
        {
            static MouseButtonCommand& PushTo(util::CommandBuffer& commandBuffer);
            static MouseButtonCommand& PopFrom(util::CommandBuffer& commandBuffer);

            uint32_t X;
            uint32_t Y;
            uint8_t ButtonID;
            bool Pressed;
        };

        struct MouseMoveCommand
        {
            static MouseMoveCommand& PushTo(util::CommandBuffer& commandBuffer);
            static MouseMoveCommand& PopFrom(util::CommandBuffer& commandBuffer);

            uint32_t X;
            uint32_t Y;
        };

        struct MouseWheelCommand
        {
            static MouseWheelCommand& PushTo(util::CommandBuffer& commandBuffer);
            static MouseWheelCommand& PopFrom(util::CommandBuffer& commandBuffer);

            uint32_t X;
            uint32_t Y;
            int32_t Offset;
        };

        struct KeyboardCommand
        {
            static KeyboardCommand& PushTo(util::CommandBuffer& commandBuffer);
            static KeyboardCommand& PopFrom(util::CommandBuffer& commandBuffer);

            uint8_t KeyID;
            bool Pressed;
        };
    }
}
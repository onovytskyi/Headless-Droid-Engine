#pragma once

namespace hd
{
    namespace util
    {
        class CommandBuffer;
        class CommandBufferReader;
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
            static WindowActivateCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static WindowActivateCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            bool Active;
        };

        struct WindowClosedCommand
        {
            static WindowClosedCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static WindowClosedCommand& ReadFrom(util::CommandBufferReader& commandBuffer);
        };

        struct WindowResizeCommand
        {
            static WindowResizeCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static WindowResizeCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            uint32_t Width;
            uint32_t Height;
        };

        struct MouseButtonCommand
        {
            static MouseButtonCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static MouseButtonCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            uint32_t X;
            uint32_t Y;
            uint8_t ButtonID;
            bool Pressed;
        };

        struct MouseMoveCommand
        {
            static MouseMoveCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static MouseMoveCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            uint32_t X;
            uint32_t Y;
        };

        struct MouseWheelCommand
        {
            static MouseWheelCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static MouseWheelCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            uint32_t X;
            uint32_t Y;
            int32_t Offset;
        };

        struct KeyboardCommand
        {
            static KeyboardCommand& WriteTo(util::CommandBuffer& commandBuffer);
            static KeyboardCommand& ReadFrom(util::CommandBufferReader& commandBuffer);

            uint8_t KeyID;
            bool Pressed;
        };
    }
}
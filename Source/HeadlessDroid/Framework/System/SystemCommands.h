#pragma once

namespace hd
{
	class CommandBuffer;
	class CommandBufferReader;

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
        static WindowActivateCommand& WriteTo(CommandBuffer& commandBuffer);
        static WindowActivateCommand& ReadFrom(CommandBufferReader& commandBuffer);

        bool Active;
    };

    struct WindowClosedCommand
    {
        static WindowClosedCommand& WriteTo(CommandBuffer& commandBuffer);
        static WindowClosedCommand& ReadFrom(CommandBufferReader& commandBuffer);
    };

    struct WindowResizeCommand
    {
        static WindowResizeCommand& WriteTo(CommandBuffer& commandBuffer);
        static WindowResizeCommand& ReadFrom(CommandBufferReader& commandBuffer);

        uint32_t Width;
        uint32_t Height;
    };

    struct MouseButtonCommand
    {
        static MouseButtonCommand& WriteTo(CommandBuffer& commandBuffer);
        static MouseButtonCommand& ReadFrom(CommandBufferReader& commandBuffer);

        uint32_t X;
        uint32_t Y;
        uint8_t ButtonID;
        bool Pressed;
    };

    struct MouseMoveCommand
    {
        static MouseMoveCommand& WriteTo(CommandBuffer& commandBuffer);
        static MouseMoveCommand& ReadFrom(CommandBufferReader& commandBuffer);

        uint32_t X;
        uint32_t Y;
    };

    struct MouseWheelCommand
    {
        static MouseWheelCommand& WriteTo(CommandBuffer& commandBuffer);
        static MouseWheelCommand& ReadFrom(CommandBufferReader& commandBuffer);

        uint32_t X;
        uint32_t Y;
        int32_t Offset;
    };

    struct KeyboardCommand
    {
        static KeyboardCommand& WriteTo(CommandBuffer& commandBuffer);
        static KeyboardCommand& ReadFrom(CommandBufferReader& commandBuffer);

        uint8_t KeyID;
        bool Pressed;
    };
}
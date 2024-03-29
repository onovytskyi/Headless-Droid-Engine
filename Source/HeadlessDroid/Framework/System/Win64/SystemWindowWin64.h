#pragma once

#if defined(HD_PLATFORM_WIN64)

namespace hd
{
	class CommandBuffer;

    class SystemWindowPlatform
    {
    public:
        SystemWindowPlatform(char8_t const* title, uint32_t width, uint32_t height);
        ~SystemWindowPlatform();

        HWND GetNativeHandle() const;

    protected:
        static LRESULT CALLBACK HandleMessageInternal(HWND windowHandle, UINT messageCode, WPARAM wParameter, LPARAM lParameter);

        struct WindowParameters
        {
            uint32_t Width;
            uint32_t Height;
            bool IsResizing;
            bool WasResized;
            bool IsMinimized;
            uint32_t StoredWidth;
            uint32_t StoredHeight;
            CommandBuffer* Commands;
        };

        HWND m_Handle;
        WindowParameters m_Params;
    };
}

#endif
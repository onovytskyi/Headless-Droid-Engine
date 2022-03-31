#pragma once

#include "Framework/System/Win64/SystemWindowWin64.h"

namespace hd
{
	class CommandBuffer;

    class SystemWindow : public SystemWindowPlatform
    {
    public:
        SystemWindow(char8_t const* title, uint32_t width, uint32_t height);
        ~SystemWindow();

        hdNoncopyable(SystemWindow)

        void SetVisible(bool value);

        void ProcessSystemEvents(CommandBuffer& systemCommands);

        uint32_t const& GetWidth();
        uint32_t const& GetHeight();
    };
}
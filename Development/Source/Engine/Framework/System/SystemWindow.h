#pragma once

#include "Engine/Framework/System/Win64/SystemWindowWin64.h"

namespace hd
{
    namespace util
    {
        class CommandQueue;
    }

    namespace sys
    {
        class SystemWindow : public SystemWindowPlatform
        {
        public:
            SystemWindow(char8_t const* title, uint32_t width, uint32_t height);
            ~SystemWindow();

            hdNoncopyable(SystemWindow)

            void SetVisible(bool value);

            void ProcessSystemEvents(util::CommandQueue& systemCommands);

            uint32_t const& GetWidth();
            uint32_t const& GetHeight();
        };
    }
}
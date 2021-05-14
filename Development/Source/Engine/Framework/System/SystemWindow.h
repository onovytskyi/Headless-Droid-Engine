#pragma once

#include "Engine/Framework/System/Win64/SystemWindowWin64.h"

namespace hd
{
    namespace util
    {
        class CommandBuffer;
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

            void ProcessSystemEvents(util::CommandBuffer& systemCommands);

            uint32_t const& GetWidth();
            uint32_t const& GetHeight();
        };
    }
}
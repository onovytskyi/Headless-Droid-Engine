#include "Config/Bootstrap.h"

#include "Framework/System/SystemWindow.h"

namespace hd
{
    namespace sys
    {
        SystemWindow::SystemWindow(char8_t const* title, uint32_t width, uint32_t height)
            : SystemWindowPlatform{ title, width, height }
        {

        }

        SystemWindow::~SystemWindow()
        {

        }
    }
}
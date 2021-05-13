#pragma once

namespace hd
{
    namespace cfg
    {
        inline constexpr char8_t const* LogFilepath() { return u8"headless_droid_log.txt"; }

        inline constexpr uint32_t MaxFrameLatency() { return 3; }
    }
}
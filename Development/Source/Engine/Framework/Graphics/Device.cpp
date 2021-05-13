#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/Graphics/Device.h"

namespace hd
{
    namespace gfx
    {
        Device::Device(Backend& backend)
            : DevicePlatform{ backend }
        {

        }

        Device::~Device()
        {

        }
    }
}
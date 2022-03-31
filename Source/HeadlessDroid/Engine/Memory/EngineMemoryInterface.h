#pragma once

#include "Framework/Memory/FrameworkMemoryInterface.h"

namespace hd
{
    class Allocator;

	Allocator& Persistent();
	Allocator& General();
}
#pragma once

#include "Engine/Config/BuildConfiguration.h"

#include "Engine/Config/DX12/PlatformIncludesDX12.h"
#include "Engine/Config/Win64/PlatformIncludesWin64.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <limits>
#include <memory_resource>
#include <mutex>
#include <new.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

#define hdMakeString2( text ) #text
#define hdMakeString( text ) hdMakeString2( text )

#define hdConcat2( first, second ) first##second
#define hdConcat( first, second ) hdConcat2(first, second)

#define hdNoncopyable(classname) \
            classname(classname const&) = delete; \
            classname(classname &&) = delete; \
            classname& operator = (classname const&) = delete; \
            classname& operator = (classname &&) = delete;

inline constexpr uint32_t MakeFourCC(uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4)
{
    return static_cast<uint32_t>(c1) | static_cast<uint32_t>(c2) << 8 | static_cast<uint32_t>(c3) << 16 | static_cast<uint32_t>(c4) << 24;
}

#include "Engine/Config/EngineConfiguration.h"

#include "Engine/Config/BaseMemoryInterface.h"
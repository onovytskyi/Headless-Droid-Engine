#pragma once

#include "Engine/Config/BuildConfiguration.h"

#include "Engine/Config/DX12/PlatformIncludesDX12.h"
#include "Engine/Config/Win64/PlatformIncludesWin64.h"

#include <cmath>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <limits>
#include <mutex>
#include <new.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <thread>
#include <type_traits>

#define hdMakeString2( text ) #text
#define hdMakeString( text ) hdMakeString2( text )

#define hdConcat2( first, second ) first##second
#define hdConcat( first, second ) hdConcat2(first, second)

#define hdNoncopyable(classname) \
            classname(classname const&) = delete; \
            classname(classname &&) = delete; \
            classname& operator = (classname const&) = delete; \
            classname& operator = (classname &&) = delete;

#include "Engine/Config/EngineConfiguration.h"
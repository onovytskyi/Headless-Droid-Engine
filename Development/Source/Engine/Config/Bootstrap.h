#pragma once

#include "Engine/Config/BuildConfiguration.h"

#include "Engine/Config/Win64/PlatformIncludesWin64.h"

#include <cmath>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <new.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#define hdMakeString2( text ) #text
#define hdMakeString( text ) hdMakeString2( text )

#define hdConcat2( first, second ) first##second
#define hdConcat( first, second ) hdConcat2(first, second)

#include "Engine/Config/EngineConfiguration.h"
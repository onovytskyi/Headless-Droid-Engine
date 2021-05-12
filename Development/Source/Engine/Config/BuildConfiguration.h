#pragma once

#if !defined(HD_BUILD_FINAL)
#   define HD_ENABLE_ASSERTS
#endif

#define HD_LOG_LEVEL_INFO 1
#define HD_LOG_LEVEL_WARNING 2
#define HD_LOG_LEVEL_ERROR 3

#if defined(HD_BUILD_DEBUG)
#   define HD_ENABLE_GFX_DEBUG
#   define HD_ENABLE_GFX_GPU_VALIDATION
#endif

#if defined(HD_BUILD_DEBUG) || defined(HD_BUILD_DEVELOPMENT)
#   define HD_ENABLE_CONSOLE_LOG
#   define HD_ENABLE_IDE_LOG
#   define HD_ENABLE_FILE_LOG
#   define HD_LOG_LEVEL HD_LOG_LEVEL_INFO
#endif

#if defined(HD_BUILD_PROFILE)
#   define HD_ENABLE_IDE_LOG
#   define HD_LOG_LEVEL HD_LOG_LEVEL_WARNING
#endif

#if defined(HD_ENABLE_CONSOLE_LOG) || defined(HD_ENABLE_IDE_LOG) || defined(HD_ENABLE_FILE_LOG)
#   define HD_ENABLE_LOG
#endif

#define HD_GRAPHICS_API_DX12
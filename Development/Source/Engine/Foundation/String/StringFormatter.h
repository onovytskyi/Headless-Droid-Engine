#pragma once

namespace hd
{
    namespace str
    {
        template<typename... Args>
        size_t CalculateBufferSize(char8_t const* format, Args... args);

        template<typename... Args>
        void Format(char8_t* output, char8_t const* format, Args... args);

        namespace detail
        {
            template<typename T, typename... Args>
            char8_t* Process(char8_t* output, char8_t const* format, T const& argument, Args... args);
            char8_t* Process(char8_t* output, char8_t const* text);

            template<typename T, typename... Args>
            size_t CalculateSize(char8_t const* format, T const& argument, Args... args);
            size_t CalculateSize(char8_t const* text);

            char8_t* ToString(char8_t* output, char8_t const* value);
            char8_t* ToString(char8_t* output, wchar_t const* value);
            char8_t* ToString(char8_t* output, int32_t value);
            char8_t* ToString(char8_t* output, int64_t value);
            char8_t* ToString(char8_t* output, uint32_t value);
            char8_t* ToString(char8_t* output, uint64_t value);
            char8_t* ToString(char8_t* output, float value);
            char8_t* ToString(char8_t* output, double value);

            template<typename T>
            char8_t* ToString(char8_t* output, T const* value);

            size_t ToSize(char8_t const* value);
            size_t ToSize(wchar_t const* value);
            size_t ToSize(int32_t value);
            size_t ToSize(int64_t value);
            size_t ToSize(uint32_t value);
            size_t ToSize(uint64_t value);
            size_t ToSize(float value);
            size_t ToSize(double value);

            template<typename T>
            size_t ToSize(T const* value);

            size_t DigitCount(uint64_t value);
        }
    };
}

#include "Engine/Foundation/String/StringFormatter.hpp"

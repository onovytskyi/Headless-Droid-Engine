#include "Engine/Config/Bootstrap.h"

#include "Engine/Foundation/String/StringFormatter.h"

#include "Engine/Foundation/String/StringConverter.h"

namespace hd
{
    namespace str
    {
        namespace detail
        {
            char8_t* Process(char8_t* output, char8_t const* text)
            {
                return ToString(output, text);
            }

            size_t CalculateSize(char8_t const* text)
            {
                //#TODO strlen is not really working for UTF8 strings.
                //But currently we're only using ASCII symbols
                return strlen(reinterpret_cast<char const*>(text));
            }

            char8_t* ToString(char8_t* output, char8_t const* value)
            {
                size_t inputSize = CalculateSize(value);
                memcpy_s(output, inputSize, value, inputSize);

                return output + inputSize;
            }

            char8_t* ToString(char8_t* output, wchar_t const* value)
            {
                auto inputSize = SizeAsNarrow(value) - 1;
                ToNarrow(value, output, inputSize);

                return output + inputSize;
            }

            char8_t* ToString(char8_t* output, int32_t value)
            {
                return ToString(output, int64_t(value));
            }

            char8_t* ToString(char8_t* output, int64_t value)
            {
                uint8_t negative = value < 0 ? 1U : 0U;
                value = std::abs(value);

                size_t digitCount = DigitCount(uint64_t(value)) + negative;

                int64_t remainedValue = value;
                for (size_t currentDigit = 0U; currentDigit < digitCount - negative; ++currentDigit)
                {
                    int64_t digitToPrint = remainedValue % 10;
                    output[digitCount - currentDigit - 1] = '0' + char8_t(digitToPrint);

                    remainedValue = remainedValue / 10;
                }

                if (negative == 1)
                {
                    output[0] = '-';
                }

                return output + digitCount;
            }

            char8_t* ToString(char8_t* output, uint32_t value)
            {
                return ToString(output, uint64_t(value));
            }

            char8_t* ToString(char8_t* output, uint64_t value)
            {
                size_t digitCount = DigitCount(value);

                uint64_t remainedValue = value;
                for (size_t currentDigit = 0U; currentDigit < digitCount; ++currentDigit)
                {
                    size_t digitToPrint = remainedValue % 10;
                    output[digitCount - currentDigit - 1] = '0' + char8_t(digitToPrint);

                    remainedValue = remainedValue / 10;
                }

                return output + digitCount;
            }

            char8_t* ToString(char8_t* output, float value)
            {
                return ToString(output, double(value));
            }

            char8_t* ToString(char8_t* output, double value)
            {
                size_t byteSize = ToSize(value);
                snprintf((char*)output, byteSize, "%.2f", value);

                byteSize -= 1;//Ignore last zero

                return output + byteSize;
            }

            size_t ToSize(char8_t const* value)
            {
                return CalculateSize(value);
            }

            size_t ToSize(wchar_t const* value)
            {
                return SizeAsNarrow(value) - 1;
            }

            size_t ToSize(int32_t value)
            {
                return ToSize(int64_t(value));
            }

            size_t ToSize(int64_t value)
            {
                uint8_t negative = value < 0 ? 1U : 0U;
                value = std::abs(value);

                return DigitCount(uint64_t(value)) + negative;
            }

            size_t ToSize(uint32_t value)
            {
                return ToSize(uint64_t(value));
            }

            size_t ToSize(uint64_t value)
            {
                return DigitCount(value);
            }

            size_t ToSize(float value)
            {
                return ToSize(double(value));
            }

            size_t ToSize(double value)
            {
                return size_t(snprintf(nullptr, 0, "%.2f", value));
            }

            size_t DigitCount(uint64_t value)
            {
                size_t count = 0;
                do
                {
                    ++count;
                    value /= 10;
                } while (value != 0);

                return count;
            }
        }
    }
}
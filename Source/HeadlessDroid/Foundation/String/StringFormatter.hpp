namespace hd
{
	template<typename... Args>
	inline size_t CalculateBufferSize(char8_t const* format, Args... args)
	{
		return CalculateSize(format, std::forward<Args>(args)...) + 1;
	}

	template<typename... Args>
	inline void Format(char8_t* output, char8_t const* format, Args... args)
	{
		char8_t* outputCursor = Process(output, format, std::forward<Args>(args)...);
		*outputCursor = '\0';
	}

	template<typename T, typename... Args>
	inline char8_t* Process(char8_t* output, char8_t const* format, T const& argument, Args... args)
	{
		char8_t* outputCursor = output;
		char8_t const* formatCursor = format;
		while (*formatCursor != '\0')
		{
			if (*formatCursor == '%')
			{
				++formatCursor;

				outputCursor = ToString(outputCursor, argument);
				outputCursor = Process(outputCursor, formatCursor, std::forward<Args>(args)...);
				break;
			}

			*outputCursor = *formatCursor;
			++formatCursor;
			++outputCursor;
		}

		return outputCursor;
	}

	template<typename T, typename... Args>
	inline size_t CalculateSize(char8_t const* format, T const& argument, Args... args)
	{
		size_t outputSize = 0;
		char8_t const* formatCursor = format;
		while (*formatCursor != '\0')
		{
			if (*formatCursor == '%')
			{
				++formatCursor;

				outputSize += ToSize(argument);
				outputSize += CalculateSize(formatCursor, std::forward<Args>(args)...);
				break;
			}

			++outputSize;
			++formatCursor;
		}

		return outputSize;
	}

	template<typename T>
	inline char8_t* ToString(char8_t* output, T const* value)
	{
		size_t bytesCount = sizeof(value);

		output[0] = '0';
		output[1] = 'x';

		for (size_t byteIdx = 0; byteIdx < bytesCount; ++byteIdx)
		{
			ptrdiff_t byteValue = reinterpret_cast<ptrdiff_t>(value) >> ((bytesCount - byteIdx - 1) * 8) & 0xFF;

			char8_t firstDigit = char8_t((byteValue >> 4));
			char8_t secondDigit = char8_t(byteValue & 0x0F);

			if (firstDigit < 10)
			{
				output[2 + byteIdx * 2 + 0] = '0' + firstDigit;
			}
			else
			{
				output[2 + byteIdx * 2 + 0] = 'A' + (firstDigit - 10);
			}

			if (secondDigit < 10)
			{
				output[2 + byteIdx * 2 + 1] = '0' + secondDigit;
			}
			else
			{
				output[2 + byteIdx * 2 + 1] = 'A' + (secondDigit - 10);
			}
		}

		return output + 2 + bytesCount * 2;
	}

	template<typename T>
	inline size_t ToSize(T const*)
	{
		return sizeof(T*) * 2 + 2; //for "0x"
	}
}

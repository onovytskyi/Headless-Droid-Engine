#include "Foundation/String/StringFormatter.h"

namespace hd
{
	template<typename... Args>
	inline void AssertHandler(bool condition, char8_t const* assertion, char8_t const* file, uint32_t line, Args... args)
	{
		if (!condition)
		{
			AssertHandlerInternal(assertion, file, line, std::forward<Args>(args)...);
		}
	}

	template<typename... Args>
	inline void AssertHandlerInternal(char8_t const* assertion, char8_t const* file, uint32_t line, char8_t const* format, Args... args)
	{
		size_t marker = g_AssertAllocator.GetMarker();

		size_t bufferSize = CalculateBufferSize(format, std::forward<Args>(args)...);

		char8_t* message = reinterpret_cast<char8_t*>(g_AssertAllocator.Allocate(bufferSize, 1));
		Format(message, format, std::forward<Args>(args)...);

		AssertHandlerFinal(assertion, file, line, message);

		g_AssertAllocator.Reset(marker);
	}
}

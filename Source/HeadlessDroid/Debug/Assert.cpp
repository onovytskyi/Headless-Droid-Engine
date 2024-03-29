#include "Config/Bootstrap.h"

#include "Debug/Assert.h"

#include "Foundation/Memory/Utils.h"
#include "Foundation/String/StringConverter.h"
#include "Foundation/String/StringFormatter.h"
#include "Debug/Log.h"
#include "Foundation/Memory/Utils.h"

static const size_t g_AssertAllocatorSize = hd::MB(2);
std::byte g_AssertAllocatorArena[g_AssertAllocatorSize];
hd::FixedLinearAllocator hd::g_AssertAllocator{ g_AssertAllocatorArena, g_AssertAllocatorSize };

namespace hd
{
	void AssertHandlerInternal(char8_t const* assertion, char8_t const* file, uint32_t line)
	{
		AssertHandlerFinal(assertion, file, line, nullptr);
	}

	void AssertHandlerFinal(char8_t const* assertion, char8_t const* file, uint32_t line, char8_t const* message)
	{
		size_t marker = g_AssertAllocator.GetMarker();

		const char8_t* errorFormat = u8"[ASSERT FAILED] %\n\t\tCondition: %, File: %(%)";
		size_t errorBufferSize = CalculateBufferSize(
			errorFormat,
			message != nullptr ? message : u8"No message",
			assertion,
			file,
			line);

		char8_t* error = reinterpret_cast<char8_t*>(g_AssertAllocator.Allocate(errorBufferSize, 1));
		Format(
			error,
			errorFormat,
			message != nullptr ? message : u8"No message",
			assertion,
			file,
			line);

		hdLogError(error);

		g_AssertAllocator.Reset(marker);

		BreakIntoDebugger();

		exit(EXIT_FAILURE);
	}
}

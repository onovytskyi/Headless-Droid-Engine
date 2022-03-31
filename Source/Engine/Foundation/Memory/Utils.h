#pragma once

namespace hd::mem
{
	constexpr size_t KB(const size_t value) { return value * 1024U; }
	constexpr size_t MB(const size_t value) { return KB(value) * 1024U; }
	constexpr size_t GB(const size_t value) { return MB(value) * 1024U; }
	constexpr size_t TB(const size_t value) { return GB(value) * 1024U; }

	constexpr size_t ToKB(const size_t value) { return value / 1024U; }
	constexpr size_t ToMB(const size_t value) { return ToKB(value) / 1024U; }
	constexpr size_t ToGB(const size_t value) { return ToMB(value) / 1024U; }
	constexpr size_t ToTB(const size_t value) { return ToGB(value) / 1024U; }

	[[nodiscard]] size_t AlignBelow(size_t value, size_t alignment);
	[[nodiscard]] void* AlignBelow(void* pointer, size_t alignment);

	template<typename T>
	T* AlignBelow(T* pointer, const size_t alignment)
	{
		return static_cast<T*>(AlignBelow(static_cast<void*>(pointer), alignment));
	}

	[[nodiscard]] size_t AlignAbove(size_t value, size_t alignment);
	[[nodiscard]] void* AlignAbove(void* pointer, size_t alignment);

	template<typename T>
	T* AlignAbove(T* pointer, const size_t alignment)
	{
		return static_cast<T*>(AlignAbove(static_cast<void*>(pointer), alignment));
	}

	[[nodiscard]] bool IsAligned(size_t value, size_t align);
	[[nodiscard]] bool IsAligned(void* pointer, size_t align);
}

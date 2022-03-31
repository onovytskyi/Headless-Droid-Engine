#include "Config/Bootstrap.h"

#include "Foundation/Memory/Utils.h"

namespace hd
{
	/**
         * \brief Align integer to the previous aligned value
         * \param value Source value
         * \param alignment Alignment in bytes
         * \return Aligned integer
         */
	size_t AlignBelow(const size_t value, const size_t alignment)
	{
		if (alignment == 0 || value == 0)
			return value;

		const size_t result = ((value - 1) / alignment) * alignment;
		return result;
	}

	/**
         * \brief Align pointer to the previous aligned address
         * \param pointer Source pointer
         * \param alignment Alignment in bytes
         * \return Alined pointer
         */
	void* AlignBelow(void* pointer, const size_t alignment)
	{
		return reinterpret_cast<void*>(AlignBelow(reinterpret_cast<size_t>(pointer), alignment));  // NOLINT(performance-no-int-to-ptr)
	}

	/**
         * \brief Align integer to the next aligned value
         * \param value Source value
         * \param alignment Alignment in bytes
         * \return Aligned integer
         */
	size_t AlignAbove(const size_t value, const size_t alignment)
	{
		if (alignment == 0 || value == 0)
			return value;

		const size_t result = ((value - 1) / alignment + 1) * alignment;
		return result;
	}

	/**
         * \brief Align pointer to the next aligned address
         * \param pointer Source pointer
         * \param alignment Alignment in bytes
         * \return Alined pointer
         */
	void* AlignAbove(void* pointer, const size_t alignment)
	{
		return reinterpret_cast<void*>(AlignAbove(reinterpret_cast<size_t>(pointer), alignment));  // NOLINT(performance-no-int-to-ptr)
	}

	/**
         * \brief Check if integer value is aligned to certain alignment
         * \param value Source value
         * \param align Alignment in bytes
         * \return True if value is aligned
         */
	bool IsAligned(const size_t value, const size_t align)
	{
		return (value % align) == 0;
	}

	/**
         * \brief Check if pointer is aligned to certain alignment
         * \param pointer Source pointer
         * \param align Alignment in bytes
         * \return True if pointer is aligned
         */
	bool IsAligned(void* pointer, const size_t align)
	{
		return IsAligned(reinterpret_cast<size_t>(pointer), align);
	}
}

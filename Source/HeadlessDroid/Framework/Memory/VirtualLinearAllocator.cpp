#include "Config/Bootstrap.h"

#include "Framework/Memory/VirtualLinearAllocator.h"

#include "Foundation/Memory/Utils.h"
#include "Debug/Assert.h"

namespace hd
{
	VirtualLinearAllocator::VirtualLinearAllocator(size_t maximumSize)
		: VirtualLinearAllocator(maximumSize, 0)
	{

	}

	VirtualLinearAllocator::VirtualLinearAllocator(size_t maximumSize, size_t initialSize)
		: VirtualLinearAllocator(maximumSize, initialSize, true)
	{

	}

	VirtualLinearAllocator::VirtualLinearAllocator(size_t maximumSize, size_t initialSize, bool autoShrink)
		: m_Memory{ maximumSize, 1 }
		  , m_UsedSize{}
		  , m_AutoShrink{ autoShrink }
	{
		if (initialSize != 0)
		{
			m_Memory.Resize(initialSize);
		}
	}

	VirtualLinearAllocator::~VirtualLinearAllocator()
	{

	}

	void* VirtualLinearAllocator::Allocate(size_t size, size_t align)
	{
		std::byte* allocation = m_Memory.GetData() + m_UsedSize;
		std::byte* alignedAllocation = reinterpret_cast<std::byte*>(AlignAbove(allocation, align));

		m_UsedSize = (alignedAllocation + size) - m_Memory.GetData();

		if (m_UsedSize > m_Memory.GetSize())
		{
			m_Memory.Resize(m_UsedSize);
		}

		return alignedAllocation;
	}

	void VirtualLinearAllocator::Deallocate(void* memory, size_t sizeInBytes, size_t alignInBytes)
	{
		// NOP
	}

	size_t VirtualLinearAllocator::GetMarker() const
	{
		return m_UsedSize;
	}

	void VirtualLinearAllocator::Reset()
	{
		Reset(0);
	}

	void VirtualLinearAllocator::Reset(size_t marker)
	{
		hdAssert(marker <= m_UsedSize, u8"Invalid marker to reset to.");
		m_UsedSize = marker;

		if (m_AutoShrink)
		{
			Shrink();
		}
	}

	void VirtualLinearAllocator::Shrink()
	{
		m_Memory.Resize(m_UsedSize);
	}
}

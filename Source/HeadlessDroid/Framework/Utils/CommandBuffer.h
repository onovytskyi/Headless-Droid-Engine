#pragma once

#include "Framework/Memory/VirtualBuffer.h"

namespace hd
{
	class CommandBuffer
	{
	public:
		friend class CommandBufferReader;

		CommandBuffer(size_t maxSizeInBytes);
		~CommandBuffer();

		hdNoncopyable(CommandBuffer)

		template<typename T>
		T& Write();

		template<typename T>
		T& Write(size_t count);

		void Clear();

		VirtualBuffer& GetBuffer();

	private:
		std::byte* WriteToVirtualBuffer(size_t size);
		std::byte* ReadFromVirtualBuffer(size_t size, size_t& offset);

		VirtualBuffer m_Buffer;
	};
}

#include "Framework/Utils/CommandBuffer.hpp"

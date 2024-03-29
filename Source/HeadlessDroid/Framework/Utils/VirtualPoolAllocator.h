#pragma once

#include "Framework/Memory/VirtualBuffer.h"

namespace hd
{
	template<typename Payload>
	class VirtualPoolAllocator
	{
	public:
		using Handle = uint64_t;

		VirtualPoolAllocator(uint32_t maxItemCount);
		~VirtualPoolAllocator();
            
		Handle Allocate(Payload** outItem);
		void Free(Handle handle);

		bool IsValid(Handle handle) const;
		Payload& Get(Handle handle);

		static constexpr Handle InvalidHandle();

	private:
		struct Item
		{
			uint32_t Version;
			union 
			{
				Payload Data;
				uint32_t NextFreeItem;
			};
		};

		VirtualBuffer m_Memory;
		uint32_t m_FirstFreeItem;
	};
}

#include "Framework/Utils/VirtualPoolAllocator.hpp"

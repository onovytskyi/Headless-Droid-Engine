#pragma once

#include "Framework/Graphics/DX12/FenceDX12.h"

namespace hd
{
	class Device;

	class Fence : public FencePlatform
	{
	public:
		Fence(Device& device, uint64_t initialValue);
		~Fence();

		hdNoncopyable(Fence)

		uint64_t GetValue() const;
		void Wait(uint64_t targetValue);
	};
}

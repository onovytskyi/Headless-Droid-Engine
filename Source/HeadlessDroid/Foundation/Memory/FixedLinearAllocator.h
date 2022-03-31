#pragma once

namespace hd
{
	class FixedLinearAllocator final : public Allocator
	{
	public:
		FixedLinearAllocator(void* memory, size_t size);
		~FixedLinearAllocator() override = default;

		hdNoncopyable(FixedLinearAllocator)

		[[nodiscard]] void* Allocate(size_t size, size_t align) override;
		void Deallocate(void* memory, size_t sizeInBytes, size_t alignInBytes) override;
		[[nodiscard]] size_t GetMarker() const;
		void Reset();
		void Reset(size_t marker);

	private:
		std::byte* m_MemoryBegin;
		std::byte* m_MemoryEnd;
		std::byte* m_MemoryPointer;
	};
}

#pragma once

namespace hd
{
	class AllocatorConcurrentAdaper : public Allocator
	{
	public:
		AllocatorConcurrentAdaper(Allocator& hostAllocator);
		~AllocatorConcurrentAdaper();

		hdNoncopyable(AllocatorConcurrentAdaper)

		void* Allocate(size_t size, size_t align) override;
		void Deallocate(void* memory, size_t size, size_t align) override;

	private:
		Allocator& m_HostAllocator;
		std::mutex m_Lock;
	};
}

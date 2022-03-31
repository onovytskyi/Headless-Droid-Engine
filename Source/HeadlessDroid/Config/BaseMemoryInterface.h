#pragma once

namespace hd
{
    class Allocator : public std::pmr::memory_resource
    {
    public:
        ~Allocator() override {};

        virtual void* Allocate(size_t sizeInBytes, size_t alignInBytes) = 0;
        virtual void Deallocate(void* memory, size_t sizeInBytes, size_t alignInBytes) = 0;

    private:
        void* do_allocate(size_t sizeInBytes, size_t alignInBytes) override { return Allocate(sizeInBytes, alignInBytes); }
        void do_deallocate(void* memory, size_t sizeInBytes, size_t alignInBytes) override { Deallocate(memory, sizeInBytes, alignInBytes); }
        bool do_is_equal(const std::pmr::memory_resource& resource) const noexcept override { return this == &resource; }
    };

    namespace mem
    {
        template<typename T>
        void* NewHelper(Allocator& allocator, char const* file, uint32_t line)
        {
            return allocator.Allocate(sizeof(T), alignof(T));
        }

        template<typename T>
        void DeleteHelper(Allocator& allocator, T* object, char const* file, uint32_t line)
        {
            object->~T();

            if constexpr (std::is_polymorphic_v<T>)
            {
                allocator.Deallocate(dynamic_cast<void*>(object), sizeof(T), alignof(T));
            }
            else
            {
                allocator.Deallocate(object, sizeof(T), alignof(T));
            }
        }

        template<typename T>
        T* AllocateHelper(Allocator& allocator, size_t count, char const* file, uint32_t line)
        {
            return reinterpret_cast<T*>(allocator.Allocate(sizeof(T) * count, alignof(T)));
        }

        template<typename T>
        void FreeHelper(Allocator& allocator, T* memory, size_t count, char const* file, uint32_t line)
        {
            allocator.Deallocate(memory, sizeof(T) * count, alignof(T));
        }
    }
}

#define hdNew(allocator, typeName) new(hd::mem::NewHelper<typeName>(allocator, __FILE__, __LINE__)) typeName
#define hdAllocate(allocator, typeName, sizeInElements) hd::mem::AllocateHelper<typeName>(allocator, sizeInElements, __FILE__, __LINE__)

#define hdDelete(allocator, object) hd::mem::DeleteHelper(allocator, object, __FILE__, __LINE__)
#define hdSafeDelete(allocator, object) if (object != nullptr) { hd::mem::DeleteHelper(allocator, object, __FILE__, __LINE__); object = nullptr; }

#define hdFree(allocator, allocation, sizeInElements) hd::mem::FreeHelper(allocator, allocation, sizeInElements, __FILE__, __LINE__)
#define hdSafeFree(allocator, allocation, sizeInElements) if (allocation != nullptr) { hd::mem::FreeHelper(allocator, allocation, sizeInElements, __FILE__, __LINE__); allocation = nullptr; }
namespace hd
{
    namespace util
    {
        template<typename T>
        inline T& CommandBuffer::Push()
        {
            return Push<T>(1);
        }

        template<typename T>
        inline T& CommandBuffer::Pop()
        {
            return Pop<T>(1);
        }

        template<typename T>
        inline T& CommandBuffer::Push(size_t count)
        {
            T* data = reinterpret_cast<T*>(WriteToVirtualBuffer(sizeof(T) * count));

            return *data;
        }

        template<typename T>
        inline T& CommandBuffer::Pop(size_t count)
        {
            T* data = reinterpret_cast<T*>(ReadFromVirtualBuffer(sizeof(T) * count));

            return *data;
        }
    }
}
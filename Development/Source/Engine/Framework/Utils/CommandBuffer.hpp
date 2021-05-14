namespace hd
{
    namespace util
    {
        template<typename T>
        inline T& CommandBuffer::Write()
        {
            return Write<T>(1);
        }

        template<typename T>
        inline T& CommandBuffer::Write(size_t count)
        {
            T* data = reinterpret_cast<T*>(WriteToVirtualBuffer(sizeof(T) * count));

            return *data;
        }
    }
}
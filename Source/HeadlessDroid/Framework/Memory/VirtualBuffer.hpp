namespace hd
{
    namespace mem
    {
        template<typename T>
        inline T VirtualBuffer::GetDataAs()
        {
            return reinterpret_cast<T>(GetData());
        }

        template<typename T>
        inline typename std::remove_pointer<T>::type const* VirtualBuffer::GetDataAs() const
        {
            return reinterpret_cast<typename std::remove_pointer<T>::type const*>(GetData());
        }
    }
}
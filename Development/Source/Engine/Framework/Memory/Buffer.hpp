namespace hd
{
    namespace mem
    {
        template<typename T>
        inline T Buffer::GetDataAs()
        {
            return reinterpret_cast<T>(GetData());
        }

        template<typename T>
        inline const typename std::remove_pointer<T>::type* Buffer::GetDataAs() const
        {
            return reinterpret_cast<const typename std::remove_pointer<T>::type*>(GetData());
        }
    }
}
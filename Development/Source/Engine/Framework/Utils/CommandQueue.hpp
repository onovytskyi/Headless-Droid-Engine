namespace hd
{
    namespace util
    {
        template<typename Command>
        inline Command& CommandQueue::PushCommand()
        {
            uint32_t* commandID = WriteToVirtualBuffer<uint32_t>();
            *commandID = uint32_t(Command::ID);

            Command* command = WriteToVirtualBuffer<Command>();

            return *command;
        }

        template<typename Command>
        inline Command& CommandQueue::PopCommand()
        {
            Command* command = ReadFromVirtualBuffer<Command>();

            return *command;
        }

        template<typename T>
        inline T* CommandQueue::WriteToVirtualBuffer()
        {
            return reinterpret_cast<T*>(WriteToVirtualBuffer(sizeof(T)));
        }

        template<typename T>
        inline T* CommandQueue::ReadFromVirtualBuffer()
        {
            return reinterpret_cast<T*>(ReadFromVirtualBuffer(sizeof(T)));
        }
    }
}
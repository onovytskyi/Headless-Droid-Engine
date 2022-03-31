namespace hd
{
	template<typename T>
	inline T& CommandBufferReader::Read()
	{
		return Read<T>(1);
	}

	template<typename T>
	inline T& CommandBufferReader::Read(size_t count)
	{
		T* data = reinterpret_cast<T*>(m_CommandBuffer.ReadFromVirtualBuffer(sizeof(T) * count, m_ReadOffset));

		return *data;
	}
}

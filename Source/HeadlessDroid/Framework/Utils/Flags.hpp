namespace hd
{
	template <typename T>
	inline Flags<T>::Flags() 
		: m_Bitfield{ 0 }
	{
	}

	template <typename T>
	inline Flags<T>::Flags(T flag)
		: m_Bitfield{ 0 }
	{
		m_Bitfield = ConvertToBit(flag);
	}

	template <typename T>
	inline Flags<T>::Flags(Flags<T> const& other)
		: m_Bitfield{ other.m_Bitfield }
	{
	}

	template <typename T>
	inline Flags<T>& Flags<T>::operator=(Flags<T> const& other)
	{
		m_Bitfield = other.m_Bitfield;
		return *this;
	}

	template <typename T>
	inline void Flags<T>::Set(T flag)
	{
		UType bit = ConvertToBit(flag);
		m_Bitfield |= bit;
	}

	template <typename T>
	inline void Flags<T>::Unset(T flag)
	{
		UType bit = ConvertToBit(flag);
		m_Bitfield &= ~bit;
	}

	template <typename T>
	inline void Flags<T>::Switch(T flag)
	{
		UType bit = ConvertToBit(flag);
		m_Bitfield ^= bit;
	}

	template <typename T>
	inline void Flags<T>::Reset()
	{
		m_Bitfield = 0;
	}

	template <typename T>
	inline bool Flags<T>::IsSet(T flag) const
	{
		UType bit = ConvertToBit(flag);

		return (m_Bitfield & bit) == bit;
	}

	template<typename T>
	inline bool Flags<T>::IsAllSet(Flags<T> const& other)
	{
		if (other.GetBitfield() == 0 && m_Bitfield == 0)
		{
			return true;
		}

		return (m_Bitfield & other.GetBitfield()) == other.GetBitfield();
	}

	template<typename T>
	inline bool Flags<T>::OnlyOneBitSet()
	{
		return m_Bitfield && !(m_Bitfield & (m_Bitfield - 1));
	}

	template <typename T>
	inline typename std::underlying_type<T>::type Flags<T>::GetBitfield() const
	{
		return m_Bitfield;
	}

	template <typename T>
	inline void Flags<T>::SetBitfield(UType bitfield)
	{
		m_Bitfield = bitfield;
	}

	template <typename T>
	inline typename Flags<T>::UType Flags<T>::ConvertToBit(T flag)
	{
		if constexpr (is_bit_flags<T>::value)
		{
			return static_cast<UType>(flag);
		}
		else
		{
			return static_cast<UType>(1) << static_cast<UType>(flag);
		}
	}
}

template <typename T>
inline hd::Flags<T> operator|(hd::Flags<T> const& first, hd::Flags<T> const& second)
{
    hd::Flags<T> result;
    result.SetBitfield(first.GetBitfield() | second.GetBitfield());
    return result;
}

template <typename T, typename>
inline hd::Flags<T> operator|(T first, T second)
{
    return hd::Flags<T>(first) | hd::Flags<T>(second);
}

template <typename T, typename>
inline hd::Flags<T> operator|(hd::Flags<T> const& first, T second)
{
    return first | hd::Flags<T>(second);
}

template <typename T, typename>
inline hd::Flags<T> operator|(T first, hd::Flags<T> const& second)
{
    return hd::Flags<T>(first) | second;
}
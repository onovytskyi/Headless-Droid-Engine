#pragma once

template<typename T> struct is_flags : std::false_type {};
template<typename T> struct is_bit_flags : std::false_type {};

namespace hd
{
	template<typename T>
	class Flags
	{
	public:
		using UType = typename std::underlying_type<T>::type;

		Flags();
		Flags(T flag);

		Flags(Flags<T> const& other);
		Flags<T>& operator=(Flags<T> const& other);

		void Set(T flag);
		void Unset(T flag);
		void Switch(T flag);

		void Reset();

		bool IsSet(T flag) const;
		bool IsAllSet(Flags<T> const& other);
		bool OnlyOneBitSet();

		UType GetBitfield() const;
		void SetBitfield(UType bitfield);

	private:
		static UType ConvertToBit(T flag);
		UType m_Bitfield;
	};
}

template<typename T>
hd::Flags<T> operator|(hd::Flags<T> const& first, hd::Flags<T> const& second);

template<typename T, typename = std::enable_if_t<is_flags<T>::value>>
hd::Flags<T> operator|(T first, T second);

template<typename T, typename = std::enable_if_t<is_flags<T>::value>>
hd::Flags<T> operator|(hd::Flags<T> const& first, T second);

template<typename T, typename = std::enable_if_t<is_flags<T>::value>>
hd::Flags<T> operator|(T first, hd::Flags<T> const& second);

#include "Framework/Utils/Flags.hpp"

#define hdAllowFlagsForEnum( name ) template<> struct is_flags<name> : std::true_type{};
#define hdAllowFlagsForBitEnum( name ) template<> struct is_flags<name> : std::true_type{}; template<> struct is_bit_flags<name> : std::true_type{};
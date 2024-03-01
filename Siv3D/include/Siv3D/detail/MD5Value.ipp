//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		constexpr uint8 MakeByte(const uint32 c0, const uint32 c1) noexcept
		{
			const uint32 n0 = ((c0 & 0xF) + ((c0 & 0x40) >> 6) * 9);
			const uint32 n1 = ((c1 & 0xF) + ((c1 & 0x40) >> 6) * 9);
			return static_cast<uint8>(n0 * 16 + n1);
		}

		template <class CharType>
		[[nodiscard]]
		constexpr Optional<MD5Value> ParseMD5(const CharType* begin, const CharType* end) noexcept
		{
			if ((end - begin) != 32)
			{
				return{};
			}

			for (const CharType* it = begin; it != end; ++it)
			{
				if (not IsXdigit(*it))
				{
					return{};
				}
			}

			std::array<uint8, 16> values;

			for (size_t i = 0; i < 16; ++i)
			{
				values[i] = MakeByte(static_cast<uint32>(begin[i * 2]), static_cast<uint32>(begin[i * 2 + 1]));
			}

			return Optional<MD5Value>{ std::in_place, values };
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr MD5Value::MD5Value() noexcept
		: m_values{{ 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e }} {}

	constexpr MD5Value::MD5Value(const std::array<uint8, 16>& md5) noexcept
		: m_values{ md5 } {}
		
	////////////////////////////////////////////////////////////////
	//
	//	isEmptyHash
	//
	////////////////////////////////////////////////////////////////

	constexpr bool MD5Value::isEmptyHash() const noexcept
	{
		return (m_values == MD5Value::EmptyHash().m_values);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	getData
	//
	////////////////////////////////////////////////////////////////

	constexpr const std::array<uint8, 16>& MD5Value::getData() const noexcept
	{
		return m_values;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	constexpr void MD5Value::swap(MD5Value& other)
	{
		m_values.swap(other.m_values);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 MD5Value::hash() const noexcept
	{
		return Hash(m_values);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	EmptyHash
	//
	////////////////////////////////////////////////////////////////

	constexpr MD5Value MD5Value::EmptyHash() noexcept
	{
		return MD5Value{ { 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e } };
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	constexpr Optional<MD5Value> MD5Value::Parse(const std::string_view md5)
	{
		return detail::ParseMD5(md5.data(), (md5.data() + md5.size()));
	}

	constexpr Optional<MD5Value> MD5Value::Parse(const StringView md5)
	{
		return detail::ParseMD5(md5.data(), (md5.data() + md5.size()));
	}
}

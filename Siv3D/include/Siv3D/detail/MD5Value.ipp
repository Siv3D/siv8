//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	namespace
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
		: m_digest{{ 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e }} {}

	constexpr MD5Value::MD5Value(const std::span<const uint8, 16> md5) noexcept
		: m_digest{ md5[0], md5[1], md5[2], md5[3], md5[4], md5[5], md5[6], md5[7],
				 md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15] } {}
		
	////////////////////////////////////////////////////////////////
	//
	//	isEmptyHash
	//
	////////////////////////////////////////////////////////////////

	constexpr bool MD5Value::isEmptyHash() const noexcept
	{
		return (m_digest == MD5Value::EmptyHash().m_digest);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	getData
	//
	////////////////////////////////////////////////////////////////

	constexpr const std::array<uint8, 16>& MD5Value::digest() const noexcept
	{
		return m_digest;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	constexpr void MD5Value::swap(MD5Value& other)
	{
		m_digest.swap(other.m_digest);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 MD5Value::hash() const noexcept
	{
		return Hash(m_digest);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	EmptyHash
	//
	////////////////////////////////////////////////////////////////

	constexpr MD5Value MD5Value::EmptyHash() noexcept
	{
		return MD5Value{ std::array<uint8, 16>{ 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e } };
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	constexpr Optional<MD5Value> MD5Value::Parse(const std::string_view md5)
	{
		return ParseMD5(md5.data(), (md5.data() + md5.size()));
	}

	constexpr Optional<MD5Value> MD5Value::Parse(const StringView md5)
	{
		return ParseMD5(md5.data(), (md5.data() + md5.size()));
	}
}

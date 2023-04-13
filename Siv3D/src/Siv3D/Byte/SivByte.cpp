//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Byte.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/IntToString.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	String ToString(const Byte value)
	{
		return ToHex(value, LetterCase::Upper);
	}

	String ToBaseN(const Byte value, const int32 radix, const LetterCase letterCase)
	{
		return ToBaseN(static_cast<uint8>(value), radix, letterCase);
	}

	String ToBinary(const Byte value)
	{
		return ToBaseN(static_cast<uint8>(value), 2);
	}

	String ToOctal(const Byte value)
	{
		return ToBaseN(static_cast<uint8>(value), 8);
	}

	String ToHex(const Byte value, const LetterCase letterCase)
	{
		const std::array<char, 2> hex = (letterCase == LetterCase::Upper) ? ToHexCharsUpperCase(value) : ToHexCharsLowerCase(value);
		return String(hex.begin(), hex.end());
	}

	std::array<char, 2> ToHexCharsLowerCase(const Byte value)
	{
		constexpr char HexMap[] = "0123456789abcdef";
		const char c0 = static_cast<char32>(HexMap[(static_cast<uint8>(value) >> 4) & 0x0F]);
		const char c1 = static_cast<char32>(HexMap[static_cast<uint8>(value) & 0x0F]);
		return{ { c0, c1 } };
	}

	std::array<char, 2> ToHexCharsUpperCase(const Byte value)
	{
		constexpr char HexMap[] = "0123456789ABCDEF";
		const char c0 = static_cast<char32>(HexMap[(static_cast<uint8>(value) >> 4) & 0x0F]);
		const char c1 = static_cast<char32>(HexMap[static_cast<uint8>(value) & 0x0F]);
		return{ { c0, c1 } };
	}

	void Formatter(FormatData& formatData, const Byte& value)
	{
		const std::array<char, 2> hex = ToHexCharsUpperCase(value);
		formatData.string.append(hex.begin(), hex.end());
	}
}

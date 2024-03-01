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

# include <iostream>
# include <Siv3D/Byte.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/ParseInt.hpp>

namespace s3d
{
	namespace detail
	{
		static constexpr char TableUpper[16] =
		{
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
		};

		static constexpr char TableLower[16] =
		{
			'0', '1', '2', '3', '4', '5', '6', '7', '8' , '9', 'a', 'b', 'c', 'd', 'e', 'f'
		};

		template <class CharType>
		[[nodiscard]]
		constexpr std::array<CharType, 2> ToHexUpperCase(const Byte value) noexcept
		{
			return{ {
				static_cast<CharType>(TableUpper[static_cast<uint8>(value) >> 4]),
				static_cast<CharType>(TableUpper[static_cast<uint8>(value) & 0xF]) } };
		}

		template <class CharType>
		[[nodiscard]]
		constexpr std::array<CharType, 2> ToHexLowerCase(const Byte value) noexcept
		{
			return{ {
				static_cast<CharType>(TableLower[static_cast<uint8>(value) >> 4]),
				static_cast<CharType>(TableLower[static_cast<uint8>(value) & 0xF]) } };
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	ToHex
	//
	////////////////////////////////////////////////////////////////

	std::array<char8, 2> ToHex(const Byte value) noexcept
	{
		return detail::ToHexUpperCase<char8>(value);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	ToHexLowerCase
	//
	////////////////////////////////////////////////////////////////

	std::array<char8, 2> ToHexLowerCase(const Byte value) noexcept
	{
		return detail::ToHexLowerCase<char8>(value);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	ToHexUpperCase
	//
	////////////////////////////////////////////////////////////////

	std::array<char8, 2> ToHexUpperCase(const Byte value) noexcept
	{
		return detail::ToHexUpperCase<char8>(value);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Byte& value)
	{
		const auto hex = detail::ToHexUpperCase<char32>(value);
		formatData.string.append(hex.begin(), hex.end());
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& output, const Byte& value)
	{
		const auto hex = detail::ToHexUpperCase<char>(value);
		return output.write(hex.data(), 2);
	}

	std::wostream& operator <<(std::wostream& output, const Byte& value)
	{
		const auto hex = detail::ToHexUpperCase<wchar_t>(value);
		return output.write(hex.data(), 2);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	std::istream& operator >>(std::istream& input, Byte& value)
	{
		std::string in;
		input >> in;

		value = Byte{ ParseInt<uint8>(in, 16) };

		return input;
	}

	std::wistream& operator >>(std::wistream& input, Byte& value)
	{
		std::wstring in;
		input >> in;

		value = Byte{ ParseInt<uint8>(Unicode::FromWstring(in), 16) };

		return input;
	}
}

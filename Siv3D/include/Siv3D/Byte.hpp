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
# include <array>
# include <cstddef> // std::byte
# include <iostream>
# include "Platform.hpp"
# include "Hash.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Byte
	//
	////////////////////////////////////////////////////////////////

	/// @brief 1 バイトのデータ | 1 byte data
	using Byte = std::byte;

	////////////////////////////////////////////////////////////////
	//
	//	AsUint8
	//
	////////////////////////////////////////////////////////////////

	/// @brief Byte 型の値を uint8 に変換します。 | Converts Byte value to uint8.
	/// @param value 変換する値 | Value to convert
	/// @return 変換した値 | Converted value
	[[nodiscard]]
	SIV3D_MSVC_INTRINSIC
	constexpr uint8 AsUint8(Byte value) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	ToHex
	//
	////////////////////////////////////////////////////////////////

	/// @brief 16 進数 00 - FF に変換します。 | Converts to 00 - FF.
	/// @param value 変換する値 | Value to convert
	/// @return 変換した値 | Converted value
	[[nodiscard]]
	std::array<char8, 2> ToHex(Byte value) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	ToHexLowerCase
	//
	////////////////////////////////////////////////////////////////
	
	/// @brief 16 進数 00 - ff に変換します。 | Converts to 00 - ff.
	/// @param value 変換する値 | Value to convert
	/// @return 変換した値 | Converted value
	[[nodiscard]]
	std::array<char8, 2> ToHexLowerCase(Byte value) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	ToHexUpperCase
	//
	////////////////////////////////////////////////////////////////

	/// @brief 16 進数 00 - FF に変換します。 | Converts to 00 - FF.
	/// @param value 変換する値 | Value to convert
	/// @return 変換した値 | Converted value
	[[nodiscard]]
	std::array<char8, 2> ToHexUpperCase(Byte value) noexcept;
}

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 1 バイトのデータを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 1 バイトのデータ
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, const Byte& value);

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& output, const Byte& value);

	std::wostream& operator <<(std::wostream& output, const Byte& value);

	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	std::istream& operator >>(std::istream& input, Byte& value);

	std::wistream& operator >>(std::wistream& input, Byte& value);
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Byte>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Byte& value) const noexcept
	{
		return s3d::Hash(value);
	}
};

# include "detail/Byte.ipp"

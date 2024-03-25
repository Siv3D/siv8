﻿//-----------------------------------------------
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
# include <ThirdParty/absl/numeric/int128.h>
# include "Common.hpp"
# include "String.hpp"
# include "LetterCase.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	int128
	//
	////////////////////////////////////////////////////////////////

	/// @brief 符号付き 128-bit 整数型 | Signed integer type with width of 128 bits
	using int128 = absl::int128;

	////////////////////////////////////////////////////////////////
	//
	//	uint128
	//
	////////////////////////////////////////////////////////////////

	/// @brief　符号無し 128-bit 整数型 | Unsigned integer type with width of 128 bits
	using uint128 = absl::uint128;

	////////////////////////////////////////////////////////////////
	//
	//	MakeInt128
	//
	////////////////////////////////////////////////////////////////

	/// @brief 上位 64-bit と下位 64-bit から 128-bit 整数を作成します。 | Creates a 128-bit integer from the upper 64 bits and lower 64 bits.
	/// @param high 上位 64-bit | Upper 64 bits
	/// @param low 下位 64-bit | Lower 64 bits
	/// @return 128-bit 整数 | 128-bit integer
	[[nodiscard]]
	constexpr int128 MakeInt128(int64 high, uint64 low) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	MakeUint128
	//
	////////////////////////////////////////////////////////////////

	/// @brief 上位 64-bit と下位 64-bit から 128-bit 整数を作成します。 | Creates a 128-bit integer from the upper 64 bits and lower 64 bits.
	/// @param high 上位 64-bit | Upper 64 bits
	/// @param low 下位 64-bit | Lower 64 bits
	/// @return 128-bit 整数 | 128-bit integer
	[[nodiscard]]
	constexpr uint128 MakeUint128(uint64 high, uint64 low) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	ToSting
	//
	////////////////////////////////////////////////////////////////

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(int128 value);

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(uint128 value);

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, int128 value);

	void Formatter(FormatData& formatData, uint128 value);
}

# include "detail/Int128.ipp"

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
# include "Common.hpp"
# include "ParseErrorReason.hpp"
# include "Optional.hpp"
# include "Result.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ParseInt
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @tparam Int 整数の型 | Integer type
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値 | Parsed integer value
	/// @throws ParseError パースに失敗した場合 | Throws if parsing fails
	template <Concept::Integral Int>
	[[nodiscard]]
	Int ParseInt(std::string_view s, int32 radix = 10);

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @tparam Int 整数の型 | Integer type
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値 | Parsed integer value
	/// @throws ParseError パースに失敗した場合 | Throws if parsing fails
	template <Concept::Integral Int>
	[[nodiscard]]
	Int ParseInt(StringView s, int32 radix = 10);

	////////////////////////////////////////////////////////////////
	//
	//	ParseIntOpt
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @tparam Int 整数の型 | Integer type
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合は none | Parsed integer value. Returns none if parsing fails
	template <Concept::Integral Int>
	[[nodiscard]]
	Optional<Int> ParseIntOpt(std::string_view s, int32 radix = 10) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @tparam Int 整数の型 | Integer type
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合は none | Parsed integer value. Returns none if parsing fails
	template <Concept::Integral Int>
	[[nodiscard]]
	Optional<Int> ParseIntOpt(StringView s, int32 radix = 10) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	ParseIntWithReason
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @tparam Int 整数の型 | Integer type
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <Concept::Integral Int>
	[[nodiscard]]
	Result<Int, ParseErrorReason> ParseIntWithReason(std::string_view s, int32 radix = 10) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @tparam Int 整数の型 | Integer type
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <Concept::Integral Int>
	[[nodiscard]]
	Result<Int, ParseErrorReason> ParseIntWithReason(StringView s, int32 radix = 10) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<int8, ParseErrorReason> ParseIntWithReason<int8>(std::string_view s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<int8, ParseErrorReason> ParseIntWithReason<int8>(StringView s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<uint8, ParseErrorReason> ParseIntWithReason<uint8>(std::string_view s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<uint8, ParseErrorReason> ParseIntWithReason<uint8>(StringView s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<int16, ParseErrorReason> ParseIntWithReason<int16>(std::string_view s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<int16, ParseErrorReason> ParseIntWithReason<int16>(StringView s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<uint16, ParseErrorReason> ParseIntWithReason<uint16>(std::string_view s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<uint16, ParseErrorReason> ParseIntWithReason<uint16>(StringView s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<int32, ParseErrorReason> ParseIntWithReason<int32>(std::string_view s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<int32, ParseErrorReason> ParseIntWithReason<int32>(StringView s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<uint32, ParseErrorReason> ParseIntWithReason<uint32>(std::string_view s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<uint32, ParseErrorReason> ParseIntWithReason<uint32>(StringView s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<int64, ParseErrorReason> ParseIntWithReason<int64>(std::string_view s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<int64, ParseErrorReason> ParseIntWithReason<int64>(StringView s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<uint64, ParseErrorReason> ParseIntWithReason<uint64>(std::string_view s, int32 radix) noexcept;

	/// @brief 文字列をパースして整数値を返します。 | Parses a string and returns an integer value.
	/// @param s パースする文字列 | String to parse
	/// @param radix 基数（2 から 36 まで） | Radix (from 2 to 36)
	/// @return パースした整数値。パースに失敗した場合はエラーの原因 | Parsed integer value. Returns the reason of the error if parsing fails
	template <>
	[[nodiscard]]
	Result<uint64, ParseErrorReason> ParseIntWithReason<uint64>(StringView s, int32 radix) noexcept;
}

# include "detail/ParseInt.ipp"

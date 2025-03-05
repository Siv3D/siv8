//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <sstream>
# include <source_location>
# include "Common.hpp"
# include "Error.hpp"
# include "Unicode.hpp"
# include "ParseBool.hpp"
# include "ParseInt.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列から Type 型の値をパースします。 | Parses a string into a value of type Type.
	/// @tparam Type パースする値の型 | Type of the value to parse
	/// @param s パースする文字列 | String to parse
	/// @param location 呼び出し元のソースコードの位置 | Source location of the caller
	/// @throws ParseError パースに失敗した場合 | If parsing fails
	/// @return パースによって得られた値 | The value obtained by parsing
	template <class Type>
	[[nodiscard]]
	Type Parse(std::string_view s, const std::source_location& location = std::source_location::current());

	/// @brief 文字列から Type 型の値をパースします。 | Parses a string into a value of type Type.
	/// @tparam Type パースする値の型 | Type of the value to parse
	/// @param s パースする文字列 | String to parse
	/// @param location 呼び出し元のソースコードの位置 | Source location of the caller
	/// @throws ParseError パースに失敗した場合 | If parsing fails
	/// @return パースによって得られた値 | The value obtained by parsing
	template <class Type>
	[[nodiscard]]
	Type Parse(StringView s, const std::source_location& location = std::source_location::current());

	////////////////////////////////////////////////////////////////
	//
	//	ParseOr
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列から Type 型の値をパースします。 | Parses a string into a value of type Type.
	/// @tparam Type パースする値の型 | Type of the value to parse
	/// @tparam U パースに失敗した場合に返す値の型 | Type of the value to return if parsing fails
	/// @param s パースする文字列 | String to parse
	/// @param defaultValue パースに失敗した場合に返す値 | Value to return if parsing fails
	/// @return パースによって得られた値。パースに失敗した場合は defaultValue | The value obtained by parsing, or defaultValue if parsing fails
	template <class Type, class U> requires std::is_convertible_v<U, Type>
	[[nodiscard]]
	Type ParseOr(std::string_view s, U&& defaultValue);

	/// @brief 文字列から Type 型の値をパースします。 | Parses a string into a value of type Type.
	/// @tparam Type パースする値の型 | Type of the value to parse
	/// @tparam U パースに失敗した場合に返す値の型 | Type of the value to return if parsing fails
	/// @param s パースする文字列 | String to parse
	/// @param defaultValue パースに失敗した場合に返す値 | Value to return if parsing fails
	/// @return パースによって得られた値。パースに失敗した場合は defaultValue | The value obtained by parsing, or defaultValue if parsing fails
	template <class Type, class U> requires std::is_convertible_v<U, Type>
	[[nodiscard]]
	Type ParseOr(StringView s, U&& defaultValue);

	////////////////////////////////////////////////////////////////
	//
	//	ParseOpt
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列から Type 型の値をパースします。 | Parses a string into a value of type Type.
	/// @tparam Type パースする値の型 | Type of the value to parse
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた値。パースに失敗した場合は none | The value obtained by parsing, or none if parsing fails
	template <class Type>
	[[nodiscard]]
	Optional<Type> ParseOpt(std::string_view s) noexcept;

	/// @brief 文字列から Type 型の値をパースします。 | Parses a string into a value of type Type.
	/// @tparam Type パースする値の型 | Type of the value to parse
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた値。パースに失敗した場合は none | The value obtained by parsing, or none if parsing fails
	template <class Type>
	[[nodiscard]]
	Optional<Type> ParseOpt(StringView s) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	ParseWithReason
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列から Type 型の値をパースします。 | Parses a string into a value of type Type.
	/// @tparam Type パースする値の型 | Type of the value to parse
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた値。パースに失敗した場合はエラーの原因 | The value obtained by parsing, or the reason for the error if parsing fails
	template <class Type>
	[[nodiscard]]
	Result<Type, ParseErrorReason> ParseWithReason(std::string_view s) noexcept;

	/// @brief 文字列から Type 型の値をパースします。 | Parses a string into a value of type Type.
	/// @tparam Type パースする値の型 | Type of the value to parse
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた値。パースに失敗した場合はエラーの原因 | The value obtained by parsing, or the reason for the error if parsing fails
	template <class Type>
	[[nodiscard]]
	Result<Type, ParseErrorReason> ParseWithReason(StringView s) noexcept;
}

# include "detail/Parse.ipp"

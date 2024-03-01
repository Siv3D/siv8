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
	//	ParseBool
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @throws ParseError パースに失敗した場合 | If parsing fails
	/// @return パースによって得られた bool 値 | The bool value obtained by parsing
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	bool ParseBool(std::string_view s);

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @throws ParseError パースに失敗した場合 | If parsing fails
	/// @return パースによって得られた bool 値 | The bool value obtained by parsing
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	bool ParseBool(StringView s);

	////////////////////////////////////////////////////////////////
	//
	//	ParseBoolOpt
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた bool 値。パースに失敗した場合は none | The bool value obtained by parsing, or none if parsing fails
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	Optional<bool> ParseBoolOpt(std::string_view s) noexcept;

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた bool 値。パースに失敗した場合は none | The bool value obtained by parsing, or none if parsing fails
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	Optional<bool> ParseBoolOpt(StringView s) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	ParseBoolWithReason
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた bool 値。パースに失敗した場合はエラーの原因 | The bool value obtained by parsing, or the reason for the error if parsing fails
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	Result<bool, ParseErrorReason> ParseBoolWithReason(std::string_view s) noexcept;

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた bool 値。パースに失敗した場合はエラーの原因 | The bool value obtained by parsing, or the reason for the error if parsing fails
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	Result<bool, ParseErrorReason> ParseBoolWithReason(StringView s) noexcept;
}

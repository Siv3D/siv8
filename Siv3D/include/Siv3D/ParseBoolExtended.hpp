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
# include <source_location>
# include "Common.hpp"
# include "ParseErrorReason.hpp"
# include "Optional.hpp"
# include "Result.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ParseBoolExtended
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @param location 呼び出し元のソースコードの位置 | Source location of the caller
	/// @throws ParseError パースに失敗した場合 | If parsing fails
	/// @return パースによって得られた bool 値 | The bool value obtained by parsing
	/// @remark `ParseBool` と異なり、Yes/No, On/Off, 1/0, y/n にも対応します。 | Unlike `ParseBool`, it also supports Yes/No, On/Off, 1/0 and y/n.
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	bool ParseBoolExtended(std::string_view s, const std::source_location& location = std::source_location::current());

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @param location 呼び出し元のソースコードの位置 | Source location of the caller
	/// @throws ParseError パースに失敗した場合 | If parsing fails
	/// @return パースによって得られた bool 値 | The bool value obtained by parsing
	/// @remark `ParseBool` と異なり、Yes/No, On/Off, 1/0, y/n にも対応します。 | Unlike `ParseBool`, it also supports Yes/No, On/Off, 1/0 and y/n.
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	bool ParseBoolExtended(StringView s, const std::source_location& location = std::source_location::current());

	////////////////////////////////////////////////////////////////
	//
	//	ParseBoolExtendedOpt
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた bool 値。パースに失敗した場合は none | The bool value obtained by parsing, or none if parsing fails
	/// @remark `ParseBoolOpt` と異なり、Yes/No, On/Off, 1/0, y/n にも対応します。 | Unlike `ParseBoolOpt`, it also supports Yes/No, On/Off, 1/0 and y/n.
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	Optional<bool> ParseBoolExtendedOpt(std::string_view s) noexcept;

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた bool 値。パースに失敗した場合は none | The bool value obtained by parsing, or none if parsing fails
	/// @remark `ParseBoolOpt` と異なり、Yes/No, On/Off, 1/0, y/n にも対応します。 | Unlike `ParseBoolOpt`, it also supports Yes/No, On/Off, 1/0 and y/n.
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	Optional<bool> ParseBoolExtendedOpt(StringView s) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	ParseBoolExtendedWithReason
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた bool 値。パースに失敗した場合はエラーの原因 | The bool value obtained by parsing, or the reason for the error if parsing fails
	/// @remark `ParseBoolWithReason` と異なり、Yes/No, On/Off, 1/0, y/n にも対応します。 | Unlike `ParseBoolWithReason`, it also supports Yes/No, On/Off, 1/0 and y/n.
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	Result<bool, ParseErrorReason> ParseBoolExtendedWithReason(std::string_view s) noexcept;

	/// @brief 文字列を bool 値にパースします。 | Parses a string into a bool value.
	/// @param s パースする文字列 | String to parse
	/// @return パースによって得られた bool 値。パースに失敗した場合はエラーの原因 | The bool value obtained by parsing, or the reason for the error if parsing fails
	/// @remark `ParseBoolWithReason` と異なり、Yes/No, On/Off, 1/0, y/n にも対応します。 | Unlike `ParseBoolWithReason`, it also supports Yes/No, On/Off, 1/0 and y/n.
	/// @remark 小文字と大文字は問いません。 | Case insensitive.
	[[nodiscard]]
	Result<bool, ParseErrorReason> ParseBoolExtendedWithReason(StringView s) noexcept;
}

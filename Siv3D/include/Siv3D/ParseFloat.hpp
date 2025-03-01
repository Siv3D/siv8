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
# include "Common.hpp"
# include "ParseErrorReason.hpp"
# include "Optional.hpp"
# include "Result.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ParseFloat
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @tparam Float 浮動小数点数の型 | Floating-point type
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数 | Parsed floating-point number
	/// @throws ParseError パースに失敗した場合 | Throws if parsing fails
	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Float ParseFloat(std::string_view s);

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数 | Parsed floating-point number
	/// @throws ParseError パースに失敗した場合 | Throws if parsing fails
	template <>
	[[nodiscard]]
	float ParseFloat<float>(std::string_view s);

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数 | Parsed floating-point number
	/// @throws ParseError パースに失敗した場合 | Throws if parsing fails
	template <>
	[[nodiscard]]
	double ParseFloat<double>(std::string_view s);

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数 | Parsed floating-point number
	/// @throws ParseError パースに失敗した場合 | Throws if parsing fails
	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Float ParseFloat(StringView s);

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数 | Parsed floating-point number
	/// @throws ParseError パースに失敗した場合 | Throws if parsing fails
	template <>
	[[nodiscard]]
	float ParseFloat<float>(StringView s);

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数 | Parsed floating-point number
	/// @throws ParseError パースに失敗した場合 | Throws if parsing fails
	template <>
	[[nodiscard]]
	double ParseFloat<double>(StringView s);

	////////////////////////////////////////////////////////////////
	//
	//	ParseFloatOpt
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @tparam Float 浮動小数点数の型 | Floating-point type
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合は none | Parsed floating-point number. Returns none if parsing fails
	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Optional<Float> ParseFloatOpt(std::string_view s) noexcept;

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合は none | Parsed floating-point number. Returns none if parsing fails
	template <>
	[[nodiscard]]
	Optional<float> ParseFloatOpt<float>(std::string_view s) noexcept;

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合は none | Parsed floating-point number. Returns none if parsing fails
	template <>
	[[nodiscard]]
	Optional<double> ParseFloatOpt<double>(std::string_view s) noexcept;

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合は none | Parsed floating-point number. Returns none if parsing fails
	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Optional<Float> ParseFloatOpt(StringView s) noexcept;

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合は none | Parsed floating-point number. Returns none if parsing fails
	template <>
	[[nodiscard]]
	Optional<float> ParseFloatOpt<float>(StringView s) noexcept;

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合は none | Parsed floating-point number. Returns none if parsing fails
	template <>
	[[nodiscard]]
	Optional<double> ParseFloatOpt<double>(StringView s) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	ParseFloatWithReason
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @tparam Float 浮動小数点数の型 | Floating-point type
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合はエラーの原因 | Parsed floating-point number. Returns the reason if parsing fails
	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Result<Float, ParseErrorReason> ParseFloatWithReason(std::string_view s) noexcept;

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @tparam Float 浮動小数点数の型 | Floating-point type
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合はエラーの原因 | Parsed floating-point number. Returns the reason if parsing fails
	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Result<Float, ParseErrorReason> ParseFloatWithReason(StringView s) noexcept;

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合はエラーの原因 | Parsed floating-point number. Returns the reason if parsing fails
	template <>
	[[nodiscard]]
	Result<float, ParseErrorReason> ParseFloatWithReason<float>(std::string_view s) noexcept;

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合はエラーの原因 | Parsed floating-point number. Returns the reason if parsing fails
	template <>
	[[nodiscard]]
	Result<float, ParseErrorReason> ParseFloatWithReason<float>(StringView s) noexcept;

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合はエラーの原因 | Parsed floating-point number. Returns the reason if parsing fails
	template <>
	[[nodiscard]]
	Result<double, ParseErrorReason> ParseFloatWithReason<double>(std::string_view s) noexcept;

	/// @brief 文字列をパースして浮動小数点数を返します。 | Parses a string and returns a floating-point number.
	/// @param s パースする文字列 | String to parse
	/// @return パースした結果の浮動小数点数。パースに失敗した場合はエラーの原因 | Parsed floating-point number. Returns the reason if parsing fails
	template <>
	[[nodiscard]]
	Result<double, ParseErrorReason> ParseFloatWithReason<double>(StringView s) noexcept;
}

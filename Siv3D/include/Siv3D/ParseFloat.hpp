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
	//	ParseFloat
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Float ParseFloat(std::string_view s);

	template <>
	[[nodiscard]]
	float ParseFloat<float>(std::string_view s);

	template <>
	[[nodiscard]]
	double ParseFloat<double>(std::string_view s);

	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Float ParseFloat(StringView s);

	template <>
	[[nodiscard]]
	float ParseFloat<float>(StringView s);

	template <>
	[[nodiscard]]
	double ParseFloat<double>(StringView s);

	////////////////////////////////////////////////////////////////
	//
	//	ParseFloatOpt
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Optional<Float> ParseFloatOpt(std::string_view s) noexcept;

	template <>
	[[nodiscard]]
	Optional<float> ParseFloatOpt<float>(std::string_view s) noexcept;

	template <>
	[[nodiscard]]
	Optional<double> ParseFloatOpt<double>(std::string_view s) noexcept;

	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Optional<Float> ParseFloatOpt(StringView s) noexcept;

	template <>
	[[nodiscard]]
	Optional<float> ParseFloatOpt<float>(StringView s) noexcept;

	template <>
	[[nodiscard]]
	Optional<double> ParseFloatOpt<double>(StringView s) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	ParseFloatWithReason
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Result<Float, ParseErrorReason> ParseFloatWithReason(std::string_view s) noexcept;

	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Result<Float, ParseErrorReason> ParseFloatWithReason(StringView s) noexcept;

	template <>
	[[nodiscard]]
	Result<float, ParseErrorReason> ParseFloatWithReason<float>(std::string_view s) noexcept;

	template <>
	[[nodiscard]]
	Result<float, ParseErrorReason> ParseFloatWithReason<float>(StringView s) noexcept;

	template <>
	[[nodiscard]]
	Result<double, ParseErrorReason> ParseFloatWithReason<double>(std::string_view s) noexcept;

	template <>
	[[nodiscard]]
	Result<double, ParseErrorReason> ParseFloatWithReason<double>(StringView s) noexcept;
}

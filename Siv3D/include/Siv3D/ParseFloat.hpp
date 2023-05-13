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

# pragma once
# include "Common.hpp"
# include "ParseErrorReason.hpp"
# include "Optional.hpp"
# include "Expected.hpp"

namespace s3d
{
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

	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Expected<Float, ParseErrorReason> ParseFloatChecked(std::string_view s) noexcept;

	template <Concept::FloatingPoint Float>
	[[nodiscard]]
	Expected<Float, ParseErrorReason> ParseFloatChecked(StringView s) noexcept;

	template <>
	[[nodiscard]]
	Expected<float, ParseErrorReason> ParseFloatChecked<float>(std::string_view s) noexcept;

	template <>
	[[nodiscard]]
	Expected<float, ParseErrorReason> ParseFloatChecked<float>(StringView s) noexcept;

	template <>
	[[nodiscard]]
	Expected<double, ParseErrorReason> ParseFloatChecked<double>(std::string_view s) noexcept;

	template <>
	[[nodiscard]]
	Expected<double, ParseErrorReason> ParseFloatChecked<double>(StringView s) noexcept;
}

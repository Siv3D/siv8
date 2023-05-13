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
	template <Concept::Integral Int>
	[[nodiscard]]
	Int ParseInt(std::string_view s, int32 radix = 10);

	template <Concept::Integral Int>
	[[nodiscard]]
	Int ParseInt(StringView s, int32 radix = 10);

	template <Concept::Integral Int>
	[[nodiscard]]
	Optional<Int> ParseIntOpt(std::string_view s, int32 radix = 10) noexcept;

	template <Concept::Integral Int>
	[[nodiscard]]
	Optional<Int> ParseIntOpt(StringView s, int32 radix = 10) noexcept;

	template <Concept::Integral Int>
	[[nodiscard]]
	Expected<Int, ParseErrorReason> ParseIntChecked(std::string_view s, int32 radix = 10) noexcept;

	template <Concept::Integral Int>
	[[nodiscard]]
	Expected<Int, ParseErrorReason> ParseIntChecked(StringView s, int32 radix = 10) noexcept;

	template <>
	[[nodiscard]]
	Expected<int8, ParseErrorReason> ParseIntChecked<int8>(std::string_view s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<int8, ParseErrorReason> ParseIntChecked<int8>(StringView s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<uint8, ParseErrorReason> ParseIntChecked<uint8>(std::string_view s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<uint8, ParseErrorReason> ParseIntChecked<uint8>(StringView s, int32 radix) noexcept;
	
	template <>
	[[nodiscard]]
	Expected<int16, ParseErrorReason> ParseIntChecked<int16>(std::string_view s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<int16, ParseErrorReason> ParseIntChecked<int16>(StringView s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<uint16, ParseErrorReason> ParseIntChecked<uint16>(std::string_view s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<uint16, ParseErrorReason> ParseIntChecked<uint16>(StringView s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<int32, ParseErrorReason> ParseIntChecked<int32>(std::string_view s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<int32, ParseErrorReason> ParseIntChecked<int32>(StringView s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<uint32, ParseErrorReason> ParseIntChecked<uint32>(std::string_view s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<uint32, ParseErrorReason> ParseIntChecked<uint32>(StringView s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<int64, ParseErrorReason> ParseIntChecked<int64>(std::string_view s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<int64, ParseErrorReason> ParseIntChecked<int64>(StringView s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<uint64, ParseErrorReason> ParseIntChecked<uint64>(std::string_view s, int32 radix) noexcept;

	template <>
	[[nodiscard]]
	Expected<uint64, ParseErrorReason> ParseIntChecked<uint64>(StringView s, int32 radix) noexcept;
}

# include "detail/ParseInt.ipp"

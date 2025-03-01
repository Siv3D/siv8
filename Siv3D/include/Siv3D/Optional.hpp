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
# include <iosfwd>
# include <optional>
# include "FormatData.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Optional
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	using Optional = std::optional<Type>;

	////////////////////////////////////////////////////////////////
	//
	//	None_t
	//
	////////////////////////////////////////////////////////////////

	/// @brief 無効値の型 | Type of invalid value
	using None_t = std::nullopt_t;

	////////////////////////////////////////////////////////////////
	//
	//	none
	//
	////////////////////////////////////////////////////////////////

	/// @brief std::nullopt の別名で、無効値を表します。 | A synonym of std::nullopt that indicates invalid value.
	inline constexpr None_t none{ std::nullopt };

	////////////////////////////////////////////////////////////////
	//
	//	unspecified
	//
	////////////////////////////////////////////////////////////////

	/// @brief none の別名で、未指定を表します。 | A synonym of none that indicates unspecified.
	inline constexpr None_t unspecified{ std::nullopt };

	////////////////////////////////////////////////////////////////
	//
	//	BadOptionalAccess
	//
	////////////////////////////////////////////////////////////////

	/// @brief 無効値を持つ Optional から値を取得しようとした際にスローされる例外 | Exception thrown when trying to get a value from an Optional with an invalid value
	using BadOptionalAccess = std::bad_optional_access;

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	std::ostream& operator <<(std::ostream& output, const None_t&);

	std::wostream& operator <<(std::wostream& output, const None_t&);

	std::basic_ostream<char32>& operator <<(std::basic_ostream<char32>& output, const None_t&);

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const None_t& value);

	template <class Type>
	void Formatter(FormatData& formatData, const Optional<Type>& value);
}

# include "detail/Optional.ipp"

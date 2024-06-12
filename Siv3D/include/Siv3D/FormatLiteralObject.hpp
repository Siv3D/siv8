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
# include <ThirdParty/fmt/format.h>
# include <ThirdParty/fmt/xchar.h>
# include <Siv3D/String.hpp>
# include <Siv3D/Format.hpp>

namespace s3d
{
	namespace detail
	{
		struct FormatLiteralObject;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Fmt
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr detail::FormatLiteralObject Fmt(const char32* s) noexcept;

	[[nodiscard]]
	constexpr detail::FormatLiteralObject Fmt(StringView s) noexcept;

	[[nodiscard]]
	constexpr detail::FormatLiteralObject Fmt(const String& s) noexcept;

	[[nodiscard]]
	constexpr detail::FormatLiteralObject Fmt(const std::u32string& s) noexcept;

	inline namespace Literals
	{
		inline namespace FormatLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_fmt
			//
			////////////////////////////////////////////////////////////////

			[[nodiscard]]
			constexpr detail::FormatLiteralObject operator ""_fmt(const char32* s, size_t length) noexcept;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	FmtFormattable
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	concept FmtFormattable = requires (Type & v, fmt::format_context ctx)
	{
		fmt::formatter<std::remove_cvref_t<Type>>().format(v, ctx);
	};
}

﻿//-----------------------------------------------
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

namespace s3d
{
	namespace detail
	{
		struct FormatHelper
		{
			fmt::basic_string_view<char32> sv;

			[[nodiscard]]
			FormatHelper() = default;

			[[nodiscard]]
			constexpr FormatHelper(const char32* s, const size_t length) noexcept
				: sv{ s, length } {}

			/// @brief フォーマット指定子 `{}` を含む文字列リテラルへ、変換する値を渡します。
			/// @tparam ...Args 変換する値の型
			/// @param ...args 変換する値
			/// @remark 文字列リテラルのあとに _fmt() サフィックスを付けると、文字列リテラル内に記述した `{}` という箇所に、( ) 内に記述した引数が文字列化されて挿入されます。
			/// @return `{}` という箇所に値を変換して挿入した文字列
			template <class... Args>
			[[nodiscard]]
			String operator()(Args&& ...args) const
			{
				return fmt::format(sv, std::forward<Args>(args)...);
			}
		};
	}

	inline auto GetFormatTag(auto& representation, auto& parseContext) -> decltype(parseContext.begin())
	{
		auto it = parseContext.begin();
		const auto itEnd = parseContext.end();

		while ((it != itEnd) && (*it != '}'))
		{
			representation.push_back(*it++);
		}

		return it;
	}

	constexpr detail::FormatHelper Fmt(const char32* s) noexcept
	{
		return{ s, std::char_traits<char32>::length(s) };
	}

	constexpr detail::FormatHelper Fmt(const StringView s) noexcept
	{
		return{ s.data(), s.size() };
	}

	inline detail::FormatHelper Fmt(const String& s) noexcept
	{
		return{ s.c_str(), s.size() };
	}

	inline detail::FormatHelper Fmt(const std::u32string& s) noexcept
	{
		return{ s.c_str(), s.size() };
	}

	namespace Literals
	{
		namespace FormatLiterals
		{
			constexpr detail::FormatHelper operator ""_fmt(const char32* s, const size_t length) noexcept
			{
				return{ s, length };
			}
		}
	}
}
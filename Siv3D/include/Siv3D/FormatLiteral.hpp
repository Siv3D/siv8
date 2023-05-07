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
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(26498)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(26450)
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(26451)
# include <ThirdParty/fmt/format.h>
# include <ThirdParty/fmt/xchar.h>
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()
SIV3D_DISABLE_MSVC_WARNINGS_POP()
# include <Siv3D/String.hpp>

namespace s3d
{
	namespace detail
	{
		struct FormatHelper;
	}

	[[nodiscard]]
	auto GetFormatTag(auto& representation, auto& parseContext) -> decltype(parseContext.begin());

	[[nodiscard]]
	constexpr detail::FormatHelper Fmt(const char32* s) noexcept;

	[[nodiscard]]
	constexpr detail::FormatHelper Fmt(StringView s) noexcept;

	[[nodiscard]]
	detail::FormatHelper Fmt(const String& s) noexcept;

	[[nodiscard]]
	detail::FormatHelper Fmt(const std::u32string& s) noexcept;

	inline namespace Literals
	{
		inline namespace FormatLiterals
		{
			[[nodiscard]]
			constexpr detail::FormatHelper operator ""_fmt(const char32* s, size_t length) noexcept;
		}
	}
}

# include "detail/FormatLiteral.ipp"

template <>
struct SIV3D_HIDDEN fmt::formatter<s3d::String>
{
	std::string tag;

	auto parse(fmt::format_parse_context& ctx)
	{
		return s3d::GetFormatTag(tag, ctx);
	}

	auto format(const s3d::String& value, auto& ctx)
	{
		const std::string utf8 = value.toUTF8();

		if (tag.empty())
		{
			return fmt::format_to(ctx.out(), "{}", utf8);
		}
		else
		{
			return fmt::vformat_to(ctx.out(), ("{:" + tag + '}'), fmt::make_format_args(utf8));
		}
	}
};

template <>
struct SIV3D_HIDDEN fmt::formatter<s3d::String, s3d::char32>
{
	std::u32string tag;

	auto parse(fmt::basic_format_parse_context<s3d::char32>& ctx)
	{
		return s3d::GetFormatTag(tag, ctx);
	}

	auto format(const s3d::String& value, auto& ctx)
	{
		const fmt::basic_string_view<s3d::char32> sv(value.data(), value.size());
		return fmt::format_to(ctx.out(), (tag.empty() ? U"{}" : (U"{:" + tag + U'}')), sv);
	}
};

template <>
struct SIV3D_HIDDEN fmt::formatter<s3d::StringView>
{
	std::string tag;

	auto parse(fmt::format_parse_context& ctx)
	{
		return s3d::GetFormatTag(tag, ctx);
	}

	auto format(const s3d::StringView& value, auto& ctx)
	{
		const std::string utf8 = value.toUTF8();

		if (tag.empty())
		{
			return fmt::format_to(ctx.out(), "{}", utf8);
		}
		else
		{
			return fmt::vformat_to(ctx.out(), ("{:" + tag + '}'), fmt::make_format_args(utf8));
		}
	}
};

template <>
struct SIV3D_HIDDEN fmt::formatter<s3d::StringView, s3d::char32>
{
	std::u32string tag;

	auto parse(basic_format_parse_context<s3d::char32>& ctx)
	{
		return s3d::GetFormatTag(tag, ctx);
	}

	auto format(const s3d::StringView& value, auto& ctx)
	{
		const fmt::basic_string_view<s3d::char32> sv(value.data(), value.size());
		return fmt::format_to(ctx.out(), (tag.empty() ? U"{}" : (U"{:" + tag + U'}')), sv);
	}
};

//template <class Type, class Allocator>
//struct SIV3D_HIDDEN fmt::formatter<s3d::Array<Type, Allocator>, s3d::char32>
//{
//	std::u32string tag;
//
//	auto parse(basic_format_parse_context<s3d::char32>& ctx)
//	{
//		return s3d::detail::GetFormatTag(tag, ctx);
//	}
//
//	auto format(const s3d::Array<Type, Allocator>& value, auto& ctx)
//	{
//		if (value.empty())
//		{
//			return format_to(ctx.out(), U"{{}}");
//		}
//
//		const std::u32string firstTag = (tag.empty() ? U"{{{}" : (U"{{{:" + tag + U"}"));
//		const std::u32string secondTag = (tag.empty() ? U", {}" : (U", {:" + tag + U"}"));
//
//		auto it = format_to(ctx.out(), firstTag, value[0]);
//
//		for (size_t i = 1; i < value.size(); ++i)
//		{
//			it = format_to(it, secondTag, value[i]);
//		}
//
//		it = format_to(it, U"}}");
//
//		return it;
//	}
//};
//
//template <class Type, class Allocator>
//struct SIV3D_HIDDEN fmt::formatter<std::vector<Type, Allocator>, s3d::char32>
//{
//	std::u32string tag;
//
//	auto parse(basic_format_parse_context<s3d::char32>& ctx)
//	{
//		return s3d::detail::GetFormatTag(tag, ctx);
//	}
//
//	auto format(const std::vector<Type, Allocator>& value, auto& ctx)
//	{
//		if (value.empty())
//		{
//			return format_to(ctx.out(), U"{{}}");
//		}
//
//		const std::u32string firstTag = (tag.empty() ? U"{{{}" : (U"{{{:" + tag + U"}"));
//		const std::u32string secondTag = (tag.empty() ? U", {}" : (U", {:" + tag + U"}"));
//
//		auto it = format_to(ctx.out(), firstTag, value[0]);
//
//		for (size_t i = 1; i < value.size(); ++i)
//		{
//			it = format_to(it, secondTag, value[i]);
//		}
//
//		it = format_to(it, U"}}");
//
//		return it;
//	}
//};
//
//template <class Type, size_t N>
//struct SIV3D_HIDDEN fmt::formatter<std::array<Type, N>, s3d::char32>
//{
//	std::u32string tag;
//
//	auto parse(basic_format_parse_context<s3d::char32>& ctx)
//	{
//		return s3d::detail::GetFormatTag(tag, ctx);
//	}
//
//	auto format(const std::array<Type, N>& value, auto& ctx)
//	{
//		if (value.empty())
//		{
//			return format_to(ctx.out(), U"{{}}");
//		}
//
//		const std::u32string firstTag = (tag.empty() ? U"{{{}" : (U"{{{:" + tag + U"}"));
//		const std::u32string secondTag = (tag.empty() ? U", {}" : (U", {:" + tag + U"}"));
//
//		auto it = format_to(ctx.out(), firstTag, value[0]);
//
//		for (size_t i = 1; i < value.size(); ++i)
//		{
//			it = format_to(it, secondTag, value[i]);
//		}
//
//		it = format_to(it, U"}}");
//
//		return it;
//	}
//};

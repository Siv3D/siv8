//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

////////////////////////////////////////////////////////////////
//
//	StringView (format_string)
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::StringView>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::StringView& value, format_context& ctx) const
	{
		return s3d::FmtHelper::FormatString(ctx, tag, value);
	}
};

////////////////////////////////////////////////////////////////
//
//	String (format_string)
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::String>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::String& value, format_context& ctx) const
	{
		return s3d::FmtHelper::FormatString(ctx, tag, value);
	}
};

////////////////////////////////////////////////////////////////
//
//	StringView
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::StringView, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::StringView& value, s3d::BufferContext& ctx) const;
};

////////////////////////////////////////////////////////////////
//
//	String
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::String, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::String& value, s3d::BufferContext& ctx) const;
};

////////////////////////////////////////////////////////////////
//
//	Type
//
////////////////////////////////////////////////////////////////

template <class Type>
	requires (s3d::Concept::Formattable<Type>
		&& (not std::is_convertible_v<Type, const s3d::char32*>)
		&& (not std::is_same_v<Type, s3d::char32>)
		&& (not s3d::FmtFormattable<Type>))
struct fmt::formatter<Type, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	s3d::BufferContext::iterator format(const Type& value, s3d::BufferContext& ctx) const
	{
		return s3d::FmtHelper::FormatString(ctx, tag, s3d::Format(value));
	}
};

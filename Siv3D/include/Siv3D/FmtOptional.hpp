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
# include "Optional.hpp"
# include "FormatLiteral.hpp"

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <class Type>
struct fmt::formatter<s3d::Optional<Type>>
{
	std::string tag;

	constexpr auto parse(format_parse_context& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	format_context::iterator format(const s3d::Optional<Type> value, format_context& ctx)
	{
		if (value.has_value())
		{
			if (tag.empty())
			{
				return fmt::format_to(ctx.out(), "(Optional){}", *value);
			}
			else
			{
				return fmt::vformat_to(ctx.out(), ("(Optional){:" + std::move(tag) + '}'), fmt::make_format_args(*value));
			}
		}
		else
		{
			return fmt::format_to(ctx.out(), "none");
		}
	}
};

template <class Type>
struct fmt::formatter<s3d::Optional<Type>, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx)
	{
		return s3d::FmtHelper::GetFormatTag(tag, ctx);
	}

	s3d::BufferContext::iterator format(const s3d::Optional<Type>& value, s3d::BufferContext& ctx)
	{
		if (value.has_value())
		{
			if (tag.empty())
			{
				return format_to(ctx.out(), U"(Optional){}", *value);
			}
			else
			{
				return format_to(ctx.out(), (U"(Optional){:" + std::move(tag) + U'}'), *value);
			}
		}
		else
		{
			return fmt::format_to(ctx.out(), U"none");
		}
	}
};

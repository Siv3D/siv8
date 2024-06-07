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

# include <Siv3D/ColorHSV.hpp>
# include <Siv3D/IntFormatter.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	adjustHue
	//
	////////////////////////////////////////////////////////////////

	Color Color::adjustHue(const double amount) const noexcept
	{
		HSV hsv{ *this };
		hsv.h += amount;
		return hsv.toColor();
	}

	////////////////////////////////////////////////////////////////
	//
	//	complemented
	//
	////////////////////////////////////////////////////////////////

	Color Color::complemented() const noexcept
	{
		HSV hsv{ *this };
		hsv.h += 180.0;
		return hsv.toColor();
	}

	////////////////////////////////////////////////////////////////
	//
	//	toHex
	//
	////////////////////////////////////////////////////////////////

	String Color::toHex() const
	{
		const uint32 rgb = ((r << 16) | (g << 8) | b);
		return U"{:0>6X}"_fmt(rgb);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Color& value)
	{
		constexpr size_t BufferSize = ((3 * 4) + 8 + 1);
		char32 buffer[BufferSize];
		char32* p = buffer;

		*(p++) = U'(';
		detail::AppendInt32(&p, value.r);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.g);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.b);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.a);
		*(p++) = U')';

		formatData.string.append(buffer, (p - buffer));
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Color>::format(const s3d::Color& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {}, {}, {})", value.r, value.g, value.b, value.a);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.r, value.g, value.b, value.a));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Color, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Color, s3d::char32>::format(const s3d::Color& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {}, {}, {})", value.r, value.g, value.b, value.a);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.r, value.g, value.b, value.a);
	}
}

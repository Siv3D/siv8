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

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/FormatLiteral.hpp>

namespace s3d
{
	void Line::ThrowPointAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Line::pointAtIndex() index out of range" };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Line& value)
	{
		formatData.string.append(U"(("_sv);
		detail::AppendFloat(formatData.string, value.start.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.start.y);
		formatData.string.append(U"), ("_sv);
		detail::AppendFloat(formatData.string, value.end.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.end.y);
		formatData.string.append(U"))"_sv);
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Line>::format(const s3d::Line& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {})", value.start, value.end);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.start, value.end));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Line, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Line, s3d::char32>::format(const s3d::Line& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {})", value.start, value.end);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.start, value.end);
	}
}

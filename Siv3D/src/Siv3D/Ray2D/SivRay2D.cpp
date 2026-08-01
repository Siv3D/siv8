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

# include <Siv3D/Ray2D.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Ray2D& value)
	{
		formatData.string.push_back(U'(');
		Formatter(formatData, value.origin);
		formatData.string.push_back(U',');
		Formatter(formatData, value.direction);
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

s3d::ParseContext::iterator fmt::formatter<s3d::Ray2D, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Ray2D, s3d::char32>::format(const s3d::Ray2D& value, s3d::BufferContext& ctx) const
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"(({}, {}), ({}, {}))",
			value.origin.x, value.origin.y, value.direction.x, value.direction.y);
	}
	else
	{
		const std::u32string format
			= (U"(({:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}))");
		return format_to(ctx.out(), format, value.origin.x, value.origin.y, value.direction.x, value.direction.y);
	}
}

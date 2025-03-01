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

# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Mat3x2& value)
	{
		formatData.string.push_back(U'(');
		Formatter(formatData, Float2{ value._11, value._12 });
		formatData.string.push_back(U',');
		Formatter(formatData, Float2{ value._21, value._22 });
		formatData.string.push_back(U',');
		Formatter(formatData, Float2{ value._31, value._32 });
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

s3d::ParseContext::iterator fmt::formatter<s3d::Mat3x2, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Mat3x2, s3d::char32>::format(const s3d::Mat3x2& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"(({}, {}), ({}, {}), ({}, {}))",
			value._11, value._12, value._21, value._22, value._31, value._32);
	}
	else
	{
		const std::u32string format
			= (U"(({:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}))");
		return format_to(ctx.out(), format, value._11, value._12, value._21, value._22, value._31, value._32);
	}
}

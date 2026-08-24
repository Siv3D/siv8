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

# include <Siv3D/Quaternion.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>

namespace s3d
{
	void Formatter(FormatData& formatData, const Quaternion& value)
	{
		const Float4 xyzw = value.toFloat4();

		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, xyzw.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, xyzw.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, xyzw.z);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, xyzw.w);
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Quaternion>::format(const s3d::Quaternion& value, fmt::format_context& ctx) const
{
	const s3d::Float4 xyzw = value.toFloat4();

	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {}, {}, {})", xyzw.x, xyzw.y, xyzw.z, xyzw.w);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(xyzw.x, xyzw.y, xyzw.z, xyzw.w));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Quaternion, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Quaternion, s3d::char32>::format(const s3d::Quaternion& value, s3d::BufferContext& ctx) const
{
	const s3d::Float4 xyzw = value.toFloat4();

	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {}, {}, {})", xyzw.x, xyzw.y, xyzw.z, xyzw.w);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, xyzw.x, xyzw.y, xyzw.z, xyzw.w);
	}
}

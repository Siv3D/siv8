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
# include <Siv3D/FloatFormatter.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		inline static double RemoveSRGBCurve(const double x) noexcept
		{
			return ((x < 0.04045) ? (x / 12.92) : std::pow((x + 0.055) / 1.055, 2.4));
		}

		[[nodiscard]]
		inline static double ApplySRGBCurve(const double x) noexcept
		{
			return ((x < 0.0031308) ? (12.92 * x) : (1.055 * std::pow(x, (1.0 / 2.4)) - 0.055));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	adjustHue
	//
	////////////////////////////////////////////////////////////////

	ColorF ColorF::adjustHue(const double amount) const noexcept
	{
		HSV hsv{ *this };
		hsv.h += amount;
		return hsv.toColorF();
	}

	////////////////////////////////////////////////////////////////
	//
	//	complemented
	//
	////////////////////////////////////////////////////////////////

	ColorF ColorF::complemented() const noexcept
	{
		HSV hsv{ *this };
		hsv.h += 180.0;
		return hsv.toColorF();
	}

	////////////////////////////////////////////////////////////////
	//
	//	removeSRGBCurve, applySRGBCurve
	//
	////////////////////////////////////////////////////////////////

	ColorF ColorF::removeSRGBCurve() const noexcept
	{
		return{	RemoveSRGBCurve(r),
				RemoveSRGBCurve(g),
				RemoveSRGBCurve(b),
				a
		};
	}

	ColorF ColorF::applySRGBCurve() const noexcept
	{
		return{	ApplySRGBCurve(r),
				ApplySRGBCurve(g),
				ApplySRGBCurve(b),
				a
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const ColorF& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.r);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.g);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.b);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.a);
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

s3d::ParseContext::iterator fmt::formatter<s3d::ColorF, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::ColorF, s3d::char32>::format(const s3d::ColorF& value, s3d::BufferContext& ctx)
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

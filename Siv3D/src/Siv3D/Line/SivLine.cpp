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
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	closestPointTo
	//
	////////////////////////////////////////////////////////////////

	Line::position_type Line::closestPointTo(const position_type pos) const noexcept
	{
		Vec2 v = (end - start);
		const double d = v.length();

		if (d == 0.0)
		{
			return start;
		}

		v /= d;
		const double t = v.dot(pos - start);

		if (t < 0.0)
		{
			return start;
		}

		if (d < t)
		{
			return end;
		}

		return (start + v * t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	RectF Line::boundingRect() const noexcept
	{
		const auto [x1, x2] = std::minmax(start.x, end.x);
		const auto [y1, y2] = std::minmax(start.y, end.y);
		return{ x1, y1, (x2 - x1), (y2 - y1) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Line& Line::draw(const ColorF& color) const
	{
		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(LineStyle::Default, start, end, 1.0f, { colorF, colorF });
		return *this;
	}

	const Line& Line::draw(const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLine(LineStyle::Default, start, end, 1.0f, { colorBegin.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	const Line& Line::draw(const double thickness, const ColorF& color) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(LineStyle::Default, start, end, static_cast<float>(thickness), { colorF, colorF });
		return *this;
	}

	const Line& Line::draw(const double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(LineStyle::Default, start, end, static_cast<float>(thickness), { colorBegin.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawUncapped
	//
	////////////////////////////////////////////////////////////////

	const Line& Line::drawUncapped(const ColorF& color) const
	{
		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(LineStyle::Uncapped, start, end, 1.0f, { colorF, colorF });
		return *this;
	}

	const Line& Line::drawUncapped(const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		SIV3D_ENGINE(Renderer2D)->addLine(LineStyle::Uncapped, start, end, 1.0f, { colorBegin.toFloat4(), colorEnd.toFloat4() });
		return *this;
	}

	const Line& Line::drawUncapped(double thickness, const ColorF& color) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		const Float4 colorF = color.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addLine(LineStyle::Uncapped, start, end, static_cast<float>(thickness), { colorF, colorF });
		return *this;
	}

	const Line& Line::drawUncapped(double thickness, const ColorF& colorBegin, const ColorF& colorEnd) const
	{
		if (thickness <= 0.0)
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addLine(LineStyle::Uncapped, start, end, static_cast<float>(thickness), { colorBegin.toFloat4(), colorEnd.toFloat4() });
		return *this;
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

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void Line::ThrowPointAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Line::pointAtIndex() index out of range" };
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

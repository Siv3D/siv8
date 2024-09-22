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

# include <Siv3D/RectF.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static double WrapLength(double length, const double perimeter) noexcept
		{
			if (length < 0.0)
			{
				length = (perimeter + std::fmod(length, perimeter));
			}
			else if (perimeter <= length)
			{
				length = std::fmod(length, perimeter);
			}

			return length;
		}

		[[nodiscard]]
		static bool IsInvalidRectFrame(const RectF& rect, const double innerThickness, const double outerThickness) noexcept
		{
			if ((rect.w < 0) || (rect.h < 0)
				|| (innerThickness < 0.0) || (outerThickness < 0.0)
				|| ((innerThickness == 0.0) && (outerThickness == 0.0)))
			{
				return true;
			}

			return false;
		}

		[[nodiscard]]
		static bool IsFilledRect(const RectF& rect, const double innerThickness) noexcept
		{
			return ((rect.w < (innerThickness * 2)) || (rect.h < (innerThickness * 2)));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtLength
	//
	////////////////////////////////////////////////////////////////

	Vec2 RectF::pointAtLength(double length) const noexcept
	{
		length = WrapLength(length, perimeter());

		if (length <= size.x)
		{
			return{ (pos.x + length), pos.y };
		}
		else if (length <= (size.x + size.y))
		{
			return{ (pos.x + size.x), (pos.y + (length - size.x)) };
		}
		else if (length <= (size.x * 2 + size.y))
		{
			return{ (pos.x + size.x - (length - size.x - size.y)), (pos.y + size.y) };
		}
		else
		{
			return{ pos.x, (pos.y + size.y - (length - size.x * 2 - size.y)) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	interpolatedPointAt
	//
	////////////////////////////////////////////////////////////////

	Vec2 RectF::interpolatedPointAt(double t) const noexcept
	{
		t = WrapLength(t, 1.0);

		const double length = (perimeter() * t);

		if (length <= size.x)
		{
			return{ (pos.x + length), pos.y };
		}
		else if (length <= (size.x + size.y))
		{
			return{ (pos.x + size.x), (pos.y + (length - size.x)) };
		}
		else if (length <= (size.x * 2 + size.y))
		{
			return{ (pos.x + size.x - (length - size.x - size.y)), (pos.y + size.y) };
		}
		else
		{
			return{ pos.x, (pos.y + size.y - (length - size.x * 2 - size.y)) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	sideIndexAtLength
	//
	////////////////////////////////////////////////////////////////

	size_t RectF::sideIndexAtLength(double length) const noexcept
	{
		length = WrapLength(length, perimeter());

		if (length <= size.x)
		{
			return 0;
		}
		else if (length <= (size.x + size.y))
		{
			return 1;
		}
		else if (length <= (size.x * 2 + size.y))
		{
			return 2;
		}
		else
		{
			return 3;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated
	//
	////////////////////////////////////////////////////////////////

	Quad RectF::rotated(const double angle) const noexcept
	{
		const double cx = (x + w * 0.5);
		const double cy = (y + h * 0.5);
		const double x0 = -size.x * 0.5;
		const double x1 = size.x * 0.5;
		const double y0 = -size.y * 0.5;
		const double y1 = size.y * 0.5;
		const double s = std::sin(angle);
		const double c = std::cos(angle);
		const double x0c = (x0 * c);
		const double x0s = (x0 * s);
		const double x1c = (x1 * c);
		const double x1s = (x1 * s);
		const double y0c = (y0 * c);
		const double y0s = (y0 * s);
		const double y1c = (y1 * c);
		const double y1s = (y1 * s);
		return{ { (x0c - y0s + cx), (x0s + y0c + cy) },
				{ (x1c - y0s + cx), (x1s + y0c + cy) },
				{ (x1c - y1s + cx), (x1s + y1c + cy) },
				{ (x0c - y1s + cx), (x0s + y1c + cy) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAt
	//
	////////////////////////////////////////////////////////////////

	Quad RectF::rotatedAt(const double _x, const double _y, const double angle) const noexcept
	{
		return rotatedAt({ _x, _y }, angle);
	}

	Quad RectF::rotatedAt(const Vec2 _pos, const double angle) const noexcept
	{
		Vec2 pts[4] = { { x, y },{ (x + w), y },{ (x + w), (y + h) },{ x, (y + h) } };

		for (int32 i = 0; i < 4; ++i)
		{
			pts[i] -= _pos;
		}

		const double s = std::sin(angle);
		const double c = std::cos(angle);

		Quad quad;

		for (int32 i = 0; i < 4; ++i)
		{
			auto& p = quad.pointAtIndex(i);
			p.x = (pts[i].x * c - pts[i].y * s + _pos.x);
			p.y = (pts[i].x * s + pts[i].y * c + _pos.y);
		}

		return quad;
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftClicked, leftPressed, leftReleased
	//
	////////////////////////////////////////////////////////////////

	bool RectF::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool RectF::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool RectF::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool RectF::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool RectF::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool RectF::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool RectF::mouseOver() const noexcept
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const RectF& RectF::draw(const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, color.toFloat4());
		return *this;
	}

	const RectF& RectF::draw(const ColorF(&colors)[4]) const
	{
		const Float4 colorsF[4] = { colors[0].toFloat4(), colors[1].toFloat4(), colors[2].toFloat4(), colors[3].toFloat4() };
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, colorsF);
		return *this;
	}

	const RectF& RectF::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		const Float4 color0 = topColor->toFloat4();
		const Float4 color1 = bottomColor->toFloat4();
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, { color0, color0, color1, color1 });
		return *this;
	}

	const RectF& RectF::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		const Float4 color0 = leftColor->toFloat4();
		const Float4 color1 = rightColor->toFloat4();
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, { color0, color1, color1, color0 });
		return *this;
	}

	const RectF& RectF::draw(const Arg::topLeft_<ColorF> topLeftColor, const Arg::bottomRight_<ColorF> bottomRightColor) const
	{
		const Float4 color0 = topLeftColor->toFloat4();
		const Float4 color2 = bottomRightColor->toFloat4();
		const Float4 color1 = ((color0 + color2) * 0.5f);
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, { color0, color1, color2, color1 });
		return *this;
	}

	const RectF& RectF::draw(const Arg::topRight_<ColorF> topRightColor, const Arg::bottomLeft_<ColorF> bottomLeftColor) const
	{
		const Float4 color0 = topRightColor->toFloat4();
		const Float4 color2 = bottomLeftColor->toFloat4();
		const Float4 color1 = ((color0 + color2) * 0.5f);
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, { color1, color0, color1, color2 });
		return *this;
	}

	const RectF& RectF::draw(const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, pattern);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrame
	//
	////////////////////////////////////////////////////////////////

	const RectF& RectF::drawFrame(const double thickness, const ColorF& color) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), color);
	}

	const RectF& RectF::drawFrame(const double thickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), innerColor, outerColor);
	}

	const RectF& RectF::drawFrame(const double thickness, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), topColor, bottomColor);
	}

	const RectF& RectF::drawFrame(const double thickness, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), leftColor, rightColor);
	}

	const RectF& RectF::drawFrame(const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		if (IsInvalidRectFrame(*this, innerThickness, outerThickness))
		{
			return *this;
		}

		if (IsFilledRect(*this, innerThickness))
		{
			stretched(outerThickness).draw(color);
			return *this;
		}

		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addRectFrame(
			FloatRect{ (x + innerThickness), (y + innerThickness), (x + w - innerThickness), (y + h - innerThickness) },
			static_cast<float>(innerThickness + outerThickness),
			color0, color0, ColorFillDirection::InOut);

		return *this;
	}

	const RectF& RectF::drawFrame(double innerThickness, const double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		if (IsInvalidRectFrame(*this, innerThickness, outerThickness))
		{
			return *this;
		}

		if (IsFilledRect(*this, innerThickness))
		{
			innerThickness = Min((w * 0.5), (h * 0.5));
		}

		SIV3D_ENGINE(Renderer2D)->addRectFrame(
			FloatRect{ (x + innerThickness), (y + innerThickness), (x + w - innerThickness), (y + h - innerThickness) },
			static_cast<float>(innerThickness + outerThickness),
			innerColor.toFloat4(), outerColor.toFloat4(), ColorFillDirection::InOut);

		return *this;
	}

	const RectF& RectF::drawFrame(const double innerThickness, const double outerThickness, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		if (IsInvalidRectFrame(*this, innerThickness, outerThickness))
		{
			return *this;
		}

		if (IsFilledRect(*this, innerThickness))
		{
			stretched(outerThickness).draw(topColor, bottomColor);
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addRectFrame(
			FloatRect{ (x + innerThickness), (y + innerThickness), (x + w - innerThickness), (y + h - innerThickness) },
			static_cast<float>(innerThickness + outerThickness),
			topColor->toFloat4(), bottomColor->toFloat4(), ColorFillDirection::TopBottom);

		return *this;
	}

	const RectF& RectF::drawFrame(const double innerThickness, const double outerThickness, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		if (IsInvalidRectFrame(*this, innerThickness, outerThickness))
		{
			return *this;
		}

		if (IsFilledRect(*this, innerThickness))
		{
			stretched(outerThickness).draw(leftColor, rightColor);
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addRectFrame(
			FloatRect{ (x + innerThickness), (y + innerThickness), (x + w - innerThickness), (y + h - innerThickness) },
			static_cast<float>(innerThickness + outerThickness),
			leftColor->toFloat4(), rightColor->toFloat4(), ColorFillDirection::LeftRight);

		return *this;
	}

	const RectF& RectF::drawFrame(const double thickness, const PatternParameters& pattern) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), pattern);
	}

	const RectF& RectF::drawFrame(const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		if (IsInvalidRectFrame(*this, innerThickness, outerThickness))
		{
			return *this;
		}

		if (IsFilledRect(*this, innerThickness))
		{
			stretched(outerThickness).draw(pattern);
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addRectFrame(
			FloatRect{ (x + innerThickness), (y + innerThickness), (x + w - innerThickness), (y + h - innerThickness) },
			static_cast<float>(innerThickness + outerThickness),
			pattern);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const RectF& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.w);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.h);
		formatData.string.push_back(U')');
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void RectF::ThrowPointAtIndexOutOfRange()
	{
		throw std::out_of_range{ "RectF::pointAtIndex() index out of range" };
	}

	void RectF::ThrowSideAtIndexOutOfRange()
	{
		throw std::out_of_range{ "RectF::sideAtIndex() index out of range" };
	}

	void RectF::ThrowTriangleAtIndexOutOfRange()
	{
		throw std::out_of_range{ "RectF::triangleAtIndex() index out of range" };
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::RectF>::format(const s3d::RectF& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {}, {}, {})", value.x, value.y, value.w, value.h);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.x, value.y, value.w, value.h));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::RectF, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::RectF, s3d::char32>::format(const s3d::RectF& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {}, {}, {})", value.x, value.y, value.w, value.h);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.x, value.y, value.w, value.h);
	}
}

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
# include <Siv3D/FormatData.hpp>
# include <Siv3D/IntFormatter.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static double WrapLength(double length, double perimeter)
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
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtLength
	//
	////////////////////////////////////////////////////////////////

	Vec2 Rect::pointAtLength(double length) const
	{
		length = WrapLength(length, perimeter());

		if (length <= size.x)
		{
			return{ pos.x + length, pos.y };
		}
		else if (length <= (size.x + size.y))
		{
			return{ pos.x + size.x, pos.y + (length - size.x) };
		}
		else if (length <= (size.x * 2 + size.y))
		{
			return{ pos.x + size.x - (length - size.x - size.y), pos.y + size.y };
		}
		else
		{
			return{ pos.x, pos.y + size.y - (length - size.x * 2 - size.y) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	interpolatedPointAt
	//
	////////////////////////////////////////////////////////////////

	Vec2 Rect::interpolatedPointAt(double t) const noexcept
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

	size_t Rect::sideIndexAtLength(double length) const
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

	Quad Rect::rotated(const double angle) const noexcept
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

	Quad Rect::rotatedAt(const double _x, const double _y, const double angle) const noexcept
	{
		return rotatedAt({ _x, _y }, angle);
	}

	Quad Rect::rotatedAt(const Vec2 _pos, const double angle) const noexcept
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
			auto& p = quad.p(i);
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

	bool Rect::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool Rect::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool Rect::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool Rect::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool Rect::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool Rect::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool Rect::mouseOver() const noexcept
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Rect& Rect::draw(const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, color.toFloat4());
		return *this;
	}

	const Rect& Rect::draw(const ColorF(&colors)[4]) const
	{
		const Float4 colorsF[4] = { colors[0].toFloat4(), colors[1].toFloat4(), colors[2].toFloat4(), colors[3].toFloat4() };
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, colorsF);
		return *this;
	}

	const Rect& Rect::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		const Float4 color0 = topColor->toFloat4();
		const Float4 color1 = bottomColor->toFloat4();
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, { color0, color0, color1, color1 });
		return *this;
	}

	const Rect& Rect::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		const Float4 color0 = leftColor->toFloat4();
		const Float4 color1 = rightColor->toFloat4();
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, { color0, color1, color1, color0 });
		return *this;
	}

	const Rect& Rect::draw(const Arg::topLeft_<ColorF> topLeftColor, const Arg::bottomRight_<ColorF> bottomRightColor) const
	{
		const Float4 color0 = topLeftColor->toFloat4();
		const Float4 color2 = bottomRightColor->toFloat4();
		const Float4 color1 = ((color0 + color2) * 0.5f);
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, { color0, color1, color2, color1 });
		return *this;
	}

	const Rect& Rect::draw(const Arg::topRight_<ColorF> topRightColor, const Arg::bottomLeft_<ColorF> bottomLeftColor) const
	{
		const Float4 color0 = topRightColor->toFloat4();
		const Float4 color2 = bottomLeftColor->toFloat4();
		const Float4 color1 = ((color0 + color2) * 0.5f);
		SIV3D_ENGINE(Renderer2D)->addRect(FloatRect{ x, y, (x + w), (y + h) }, { color1, color0, color1, color2 });
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Rect& value)
	{
		constexpr size_t BufferSize = ((detail::Int32Width * 4) + 8 + 1);
		char32 buffer[BufferSize];
		char32* p = buffer;

		*(p++) = U'(';
		detail::AppendInt32(&p, value.x);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.y);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.w);
		*(p++) = U',';
		*(p++) = U' ';
		detail::AppendInt32(&p, value.h);
		*(p++) = U')';

		formatData.string.append(buffer, (p - buffer));
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void Rect::ThrowPointAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Rect::pointAtIndex() index out of range" };
	}

	void Rect::ThrowSideAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Rect::sideAtIndex() index out of range" };
	}

	void Rect::ThrowTriangleAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Rect::triangleAtIndex() index out of range" };
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Rect>::format(const s3d::Rect& value, fmt::format_context& ctx)
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

s3d::ParseContext::iterator fmt::formatter<s3d::Rect, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Rect, s3d::char32>::format(const s3d::Rect& value, s3d::BufferContext& ctx)
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

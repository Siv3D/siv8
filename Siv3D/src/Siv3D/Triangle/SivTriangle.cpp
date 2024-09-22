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
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		//
		// http://imagingsolution.blog107.fc2.com/blog-entry-137.html
		//
		[[nodiscard]]
		static constexpr Vec2 GetP(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3) noexcept
		{
			const double a = ((p3.x - p2.x) * (p0.y - p2.y) - (p3.y - p2.y) * (p0.x - p2.x));
			const double b = ((p3.x - p2.x) * (p2.y - p1.y) - (p3.y - p2.y) * (p2.x - p1.x));
			const double c = (a / (a + b));
			return{ (p0.x + (p1.x - p0.x) * c), (p0.y + (p1.y - p0.y) * c) };
		}

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
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Triangle::Triangle(const value_type sides, const value_type angle) noexcept
		: p0{ Circular{ (Math::InvSqrt3 * sides), angle } }
		, p1{ Circular{ (Math::InvSqrt3 * sides), (2 * Math::OneThirdPi + angle) } }
		, p2{ Circular{ (Math::InvSqrt3 * sides), (4 * Math::OneThirdPi + angle) } } {}

	////////////////////////////////////////////////////////////////
	//
	//	stretched
	//
	////////////////////////////////////////////////////////////////

	Triangle Triangle::stretched(const value_type size) const noexcept
	{
		Line lines[3] =
		{
			{ p0, p1 }, { p1, p2 }, { p2, p0 }
		};

		for (auto& line : lines)
		{
			const Vec2 v = (line.end - line.start).withLength(size);
			line.moveBy(v.y, -v.x);
		}

		const Vec2 t0 = GetP(lines[2].start, lines[2].end, lines[0].start, lines[0].end);
		const Vec2 t1 = GetP(lines[0].start, lines[0].end, lines[1].start, lines[1].end);
		const Vec2 t2 = GetP(lines[1].start, lines[1].end, lines[2].start, lines[2].end);
		return{ t0, t1, t2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated
	//
	////////////////////////////////////////////////////////////////

	Triangle Triangle::rotated(const value_type angle) const noexcept
	{
		return rotatedAt(centroid(), angle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAt
	//
	////////////////////////////////////////////////////////////////

	Triangle Triangle::rotatedAt(const value_type x, const value_type y, const value_type angle) const noexcept
	{
		return rotatedAt(position_type{ x, y }, angle);
	}

	Triangle Triangle::rotatedAt(const position_type pos, const value_type angle) const noexcept
	{
		const position_type a0 = (p0 - pos);
		const position_type a1 = (p1 - pos);
		const position_type a2 = (p2 - pos);
		const value_type s = std::sin(angle);
		const value_type c = std::cos(angle);
		const position_type x0{ (a0.x * c - a0.y * s), (a0.x * s + a0.y * c) };
		const position_type x1{ (a1.x * c - a1.y * s), (a1.x * s + a1.y * c) };
		const position_type x2{ (a2.x * c - a2.y * s), (a2.x * s + a2.y * c) };
		return{ (x0 + pos), (x1 + pos), (x2 + pos) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtLength
	//
	////////////////////////////////////////////////////////////////

	Vec2 Triangle::pointAtLength(double length) const noexcept
	{
		const double l0 = p1.distanceFrom(p0);
		const double l1 = p2.distanceFrom(p1);
		const double l2 = p0.distanceFrom(p2);

		length = WrapLength(length, (l0 + l1 + l2));

		if (length <= l0)
		{
			return Line{ p0, p1 }.pointAtLength(length);
		}
		else if (length <= (l0 + l1))
		{
			return Line{ p1, p2 }.pointAtLength(length - l0);
		}
		else
		{
			return Line{ p2, p0 }.pointAtLength(length - l0 - l1);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	interpolatedPointAt
	//
	////////////////////////////////////////////////////////////////

	Vec2 Triangle::interpolatedPointAt(double t) const noexcept
	{
		t = WrapLength(t, 1.0);

		const double l0 = p1.distanceFrom(p0);
		const double l1 = p2.distanceFrom(p1);
		const double l2 = p0.distanceFrom(p2);
		const double length = ((l0 + l1 + l2) * t);

		if (length <= l0)
		{
			return Line{ p0, p1 }.pointAtLength(length);
		}
		else if (length <= (l0 + l1))
		{
			return Line{ p1, p2 }.pointAtLength(length - l0);
		}
		else
		{
			return Line{ p2, p0 }.pointAtLength(length - l0 - l1);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	sideIndexAtLength
	//
	////////////////////////////////////////////////////////////////

	size_t Triangle::sideIndexAtLength(double length) const noexcept
	{
		const double l0 = p1.distanceFrom(p0);
		const double l1 = p2.distanceFrom(p1);
		const double l2 = p0.distanceFrom(p2);
		length = WrapLength(length, (l0 + l1 + l2));

		if (length <= l0)
		{
			return 0;
		}
		else if (length <= (l0 + l1))
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	Triangle::value_type Triangle::perimeter() const noexcept
	{
		const double l0 = p1.distanceFrom(p0);
		const double l1 = p2.distanceFrom(p1);
		const double l2 = p0.distanceFrom(p2);
		return (l0 + l1 + l2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	RectF Triangle::boundingRect() const noexcept
	{
		auto [xMin, xMax] = std::minmax({ p0.x, p1.x, p2.x });
		auto [yMin, yMax] = std::minmax({ p0.y, p1.y, p2.y });
		return{ xMin, yMin, (xMax - xMin), (yMax - yMin) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCircumscribedCircle
	//
	////////////////////////////////////////////////////////////////

	Circle Triangle::getCircumscribedCircle() const noexcept
	{
		const double x0 = p0.x;
		const double y0 = p0.y;
		const double x1 = p1.x;
		const double y1 = p1.y;
		const double x2 = p2.x;
		const double y2 = p2.y;
		const double x0_2 = (x0 * x0);
		const double y0_2 = (y0 * y0);
		const double x1_2 = (x1 * x1);
		const double y1_2 = (y1 * y1);
		const double x2_2 = (x2 * x2);
		const double y2_2 = (y2 * y2);
		const double c = 2 * ((x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0));
		const Vec2 center
		{
			((y2 - y0) * (x1_2 - x0_2 + y1_2 - y0_2) + (y0 - y1) * (x2_2 - x0_2 + y2_2 - y0_2)) / c,
			((x0 - x2) * (x1_2 - x0_2 + y1_2 - y0_2) + (x1 - x0) * (x2_2 - x0_2 + y2_2 - y0_2)) / c
		};

		return{ center, center.distanceFrom(p0) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getInscribedCircle
	//
	////////////////////////////////////////////////////////////////

	Circle Triangle::getInscribedCircle() const noexcept
	{
		const double d0 = p2.distanceFrom(p1);
		const double d1 = p0.distanceFrom(p2);
		const double d2 = p1.distanceFrom(p0);
		const double perimeter = (d0 + d1 + d2);
		const Vec2 center{ (p0 * d0 + p1 * d1 + p2 * d2) / perimeter };
		const double area2 = Abs((p0.x - p2.x) * (p1.y - p0.y) - (p0.x - p1.x) * (p2.y - p0.y));

		return{ center, (area2 / perimeter) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftClicked, leftPressed, leftReleased
	//
	////////////////////////////////////////////////////////////////

	bool Triangle::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool Triangle::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool Triangle::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool Triangle::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool Triangle::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool Triangle::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool Triangle::mouseOver() const noexcept
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Triangle& Triangle::draw(const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addTriangle({ p0, p1, p2 }, color.toFloat4());
		return *this;
	}

	const Triangle& Triangle::draw(const ColorF& color0, const ColorF& color1, const ColorF& color2) const
	{
		const Float4 colors[3] = { color0.toFloat4(), color1.toFloat4(), color2.toFloat4() };
		SIV3D_ENGINE(Renderer2D)->addTriangle({ p0, p1, p2 }, colors);
		return *this;
	}

	const Triangle& Triangle::draw(const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addTriangle({ p0, p1, p2 }, pattern);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromBaseCenter
	//
	////////////////////////////////////////////////////////////////

	Triangle Triangle::FromBaseCenter(const position_type& baseCenter, const position_type& top, const double baseLength) noexcept
	{
		Vec2 right = (top - baseCenter);

		right.set(-right.y, right.x).setLength(baseLength * 0.5);

		return{ top, (baseCenter + right), (baseCenter - right) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Triangle& value)
	{
		formatData.string.append(U"(("_sv);
		detail::AppendFloat(formatData.string, value.p0.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.p0.y);
		formatData.string.append(U"), ("_sv);
		detail::AppendFloat(formatData.string, value.p1.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.p1.y);
		formatData.string.append(U"), ("_sv);
		detail::AppendFloat(formatData.string, value.p2.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.p2.y);
		formatData.string.append(U"))"_sv);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void Triangle::ThrowPointAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Triangle::pointAtIndex() index out of range" };
	}

	void Triangle::ThrowSideAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Triangle::sideAtIndex() index out of range" };
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Triangle>::format(const s3d::Triangle& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "(({}, {}), ({}, {}), ({}, {}))", value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y);
	}
	else
	{
		const std::string format
			= ("(({:" + tag + "}, {:" + tag + "}), ({:" + tag + "}, {:" + tag + "}), ({:" + tag + "}, {:" + tag + "}))");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Triangle, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Triangle, s3d::char32>::format(const s3d::Triangle& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"(({}, {}), ({}, {}), ({}, {}))", value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y);
	}
	else
	{
		const std::u32string format
			= (U"(({:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}))");
		return format_to(ctx.out(), format, value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y);
	}
}

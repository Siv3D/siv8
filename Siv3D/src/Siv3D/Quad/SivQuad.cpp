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
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/Polygon/PolygonBuffer.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Polygon/ClosedLineString.hpp>

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
			const double a = (p3.x - p2.x) * (p0.y - p2.y) - (p3.y - p2.y) * (p0.x - p2.x);
			const double b = (p3.x - p2.x) * (p2.y - p1.y) - (p3.y - p2.y) * (p2.x - p1.x);
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
	//	stretched
	//
	////////////////////////////////////////////////////////////////

	Quad Quad::stretched(const value_type size) const noexcept
	{
		Line lines[4] =
		{
			{ p0, p1 },{ p1, p2 },{ p2, p3 },{ p3, p0 }
		};

		for (auto& line : lines)
		{
			const Vec2 v = (line.end - line.start).withLength(size);
			line.moveBy(v.y, -v.x);
		}

		const Vec2 t0 = GetP(lines[3].start, lines[3].end, lines[0].start, lines[0].end);
		const Vec2 t1 = GetP(lines[0].start, lines[0].end, lines[1].start, lines[1].end);
		const Vec2 t2 = GetP(lines[1].start, lines[1].end, lines[2].start, lines[2].end);
		const Vec2 t3 = GetP(lines[2].start, lines[2].end, lines[3].start, lines[3].end);
		return{ t0, t1, t2, t3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	stretchedPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon Quad::stretchedPolygon(const value_type size) const noexcept
	{
		return CalculatePolygonBuffer({ p0, p1, p2, p3 }, size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAt
	//
	////////////////////////////////////////////////////////////////

	Quad Quad::rotatedAt(const value_type x, const value_type y, const value_type angle) const noexcept
	{
		return rotatedAt(position_type{ x, y }, angle);
	}

	Quad Quad::rotatedAt(const position_type pos, const value_type angle) const noexcept
	{
		const position_type a0 = (p0 - pos);
		const position_type a1 = (p1 - pos);
		const position_type a2 = (p2 - pos);
		const position_type a3 = (p3 - pos);
		const value_type s = std::sin(angle);
		const value_type c = std::cos(angle);
		const position_type x0{ (a0.x * c - a0.y * s), (a0.x * s + a0.y * c) };
		const position_type x1{ (a1.x * c - a1.y * s), (a1.x * s + a1.y * c) };
		const position_type x2{ (a2.x * c - a2.y * s), (a2.x * s + a2.y * c) };
		const position_type x3{ (a3.x * c - a3.y * s), (a3.x * s + a3.y * c) };

		return{ (x0 + pos), (x1 + pos), (x2 + pos), (x3 + pos) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtLength
	//
	////////////////////////////////////////////////////////////////

	Vec2 Quad::pointAtLength(double length) const noexcept
	{
		const double l0 = p1.distanceFrom(p0);
		const double l1 = p2.distanceFrom(p1);
		const double l2 = p3.distanceFrom(p2);
		const double l3 = p0.distanceFrom(p3);

		length = WrapLength(length, (l0 + l1 + l2 + l3));

		if (length <= l0)
		{
			return Line{ p0, p1 }.pointAtLength(length);
		}
		else if (length <= (l0 + l1))
		{
			return Line{ p1, p2 }.pointAtLength(length - l0);
		}
		else if (length <= (l0 + l1 + l2))
		{
			return Line{ p2, p3 }.pointAtLength(length - (l0 + l1));
		}
		else
		{
			return Line{ p3, p0 }.pointAtLength(length - (l0 + l1 + l2));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	interpolatedPointAt
	//
	////////////////////////////////////////////////////////////////

	Vec2 Quad::interpolatedPointAt(double t) const noexcept
	{
		t = WrapLength(t, 1.0);

		const double l0 = p1.distanceFrom(p0);
		const double l1 = p2.distanceFrom(p1);
		const double l2 = p3.distanceFrom(p2);
		const double l3 = p0.distanceFrom(p3);
		const double length = ((l0 + l1 + l2 + l3) * t);

		if (length <= l0)
		{
			return Line{ p0, p1 }.pointAtLength(length);
		}
		else if (length <= (l0 + l1))
		{
			return Line{ p1, p2 }.pointAtLength(length - l0);
		}
		else if (length <= (l0 + l1 + l2))
		{
			return Line{ p2, p3 }.pointAtLength(length - (l0 + l1));
		}
		else
		{
			return Line{ p3, p0 }.pointAtLength(length - (l0 + l1 + l2));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	sideIndexAtLength
	//
	////////////////////////////////////////////////////////////////

	size_t Quad::sideIndexAtLength(double length) const noexcept
	{
		const double l0 = p1.distanceFrom(p0);
		const double l1 = p2.distanceFrom(p1);
		const double l2 = p3.distanceFrom(p2);
		const double l3 = p0.distanceFrom(p3);
		length = WrapLength(length, (l0 + l1 + l2 + l3));

		if (length <= l0)
		{
			return 0;
		}
		else if (length <= (l0 + l1))
		{
			return 1;
		}
		else if (length <= (l0 + l1 + l2))
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
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	Quad::value_type Quad::perimeter() const noexcept
	{
		const double l0 = p1.distanceFrom(p0);
		const double l1 = p2.distanceFrom(p1);
		const double l2 = p3.distanceFrom(p2);
		const double l3 = p0.distanceFrom(p3);
		return (l0 + l1 + l2 + l3);
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	RectF Quad::boundingRect() const noexcept
	{
		const auto [xMin, xMax] = std::minmax({ p0.x, p1.x, p2.x, p3.x });
		const auto [yMin, yMax] = std::minmax({ p0.y, p1.y, p2.y, p3.y });
		return{ xMin, yMin, (xMax - xMin), (yMax - yMin) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingCircle
	//
	////////////////////////////////////////////////////////////////






	////////////////////////////////////////////////////////////////
	//
	//	outer
	//
	////////////////////////////////////////////////////////////////

	Array<Vec2> Quad::outer() const
	{
		return{ p0, p1, p2, p3 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	asPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon Quad::asPolygon() const
	{
		return Polygon{ { p0, p1, p2, p3 },
			{ { 0, 1, 3 }, { 3, 1, 2 } },
			boundingRect(),
			SkipValidation::Yes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rounded
	//
	////////////////////////////////////////////////////////////////

	Polygon Quad::rounded(const double round, const QualityFactor& qualityFactor) const
	{
		if (round <= 0.0)
		{
			return asPolygon();
		}

		return CalculateRoundedQuad(*this, round, qualityFactor);
	}

	////////////////////////////////////////////////////////////////
	//
	//	calculateRoundBuffer
	//
	////////////////////////////////////////////////////////////////

	Polygon Quad::calculateRoundBuffer(const double distance, const QualityFactor& qualityFactor) const
	{
		return CalculatePolygonRoundBuffer({ p0, p1, p2, p3 }, distance, qualityFactor);
	}





	////////////////////////////////////////////////////////////////
	//
	//	leftClicked, leftPressed, leftReleased
	//
	////////////////////////////////////////////////////////////////

	bool Quad::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool Quad::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool Quad::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool Quad::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool Quad::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool Quad::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool Quad::mouseOver() const noexcept
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Quad& Quad::draw(const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addQuad(FloatQuad{ *this }, color.toFloat4());
		return *this;
	}

	const Quad& Quad::draw(const ColorF& color0, const ColorF& color1, const ColorF& color2, const ColorF& color3) const
	{
		const Float4 colors[4] = { color0.toFloat4(), color1.toFloat4(), color2.toFloat4(), color3.toFloat4() };
		SIV3D_ENGINE(Renderer2D)->addQuad(FloatQuad{ *this }, colors);
		return *this;
	}

	const Quad& Quad::draw(const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addQuad(FloatQuad{ *this }, pattern);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrame
	//
	////////////////////////////////////////////////////////////////

	const Quad& Quad::drawFrame(const double thickness, const ColorF& color, const JoinStyle joinStyle) const
	{
		DrawClosedLineString({ p0, p1, p2, p3, p0, p1 }, joinStyle, thickness, color);
		return *this;
	}

	const Quad& Quad::drawFrame(const double thickness, const PatternParameters& pattern, const JoinStyle joinStyle) const
	{
		DrawClosedLineString({ p0, p1, p2, p3, p0, p1 }, joinStyle, thickness, pattern);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Quad& value)
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
		formatData.string.append(U"), ("_sv);
		detail::AppendFloat(formatData.string, value.p3.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.p3.y);
		formatData.string.append(U"))"_sv);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void Quad::ThrowPointAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Quad::pointAtIndex() index out of range" };
	}

	void Quad::ThrowSideAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Quad::sideAtIndex() index out of range" };
	}

	void Quad::ThrowTriangleAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Quad::triangleAtIndex() index out of range" };
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Quad>::format(const s3d::Quad& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "(({}, {}), ({}, {}), ({}, {}), ({}, {}))", value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y, value.p3.x, value.p3.y);
	}
	else
	{
		const std::string format
			= ("(({:" + tag + "}, {:" + tag + "}), ({:" + tag + "}, {:" + tag + "}), ({:" + tag + "}, {:" + tag + "}), ({:" + tag + "}, {:" + tag + "}))");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y, value.p3.x, value.p3.y));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Quad, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Quad, s3d::char32>::format(const s3d::Quad& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"(({}, {}), ({}, {}), ({}, {}), ({}, {}))", value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y, value.p3.x, value.p3.y);
	}
	else
	{
		const std::u32string format
			= (U"(({:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}))");
		return format_to(ctx.out(), format, value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y, value.p3.x, value.p3.y);
	}
}

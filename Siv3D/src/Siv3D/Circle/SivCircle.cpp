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
# include <Siv3D/Utility.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/LineCap.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr float ClampAngle(const double angle) noexcept
		{
			return Clamp(static_cast<float>(angle), -Math::TwoPiF, Math::TwoPiF);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Circle::Circle(const position_type& p0, const position_type& p1, const position_type& p2) noexcept
	{
		if (p0 == p1)
		{
			*this = Circle{ p0, p2 };
			return;
		}
		else if ((p0 == p2) || (p1 == p2))
		{
			*this = Circle{ p0, p1 };
			return;
		}

		const double a02 = (2 * (p0.x - p2.x));
		const double b02 = (2 * (p0.y - p2.y));
		const double c02 = (p0.y * p0.y - p2.y * p2.y) + (p0.x * p0.x - p2.x * p2.x);
		const double a12 = (2 * (p1.x - p2.x));
		const double b12 = (2 * (p1.y - p2.y));
		const double c12 = (p1.y * p1.y - p2.y * p2.y) + (p1.x * p1.x - p2.x * p2.x);
		const double cy = (a02 * c12 - a12 * c02) / (a02 * b12 - a12 * b02);
		const double cx = ((Abs(a02) < Abs(a12)) ? ((c12 - b12 * cy) / a12) : ((c02 - b02 * cy) / a02));
		*this = Circle{ cx, cy, p0.distanceFrom(cx, cy) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPointByAngle
	//
	////////////////////////////////////////////////////////////////

	Circle::position_type Circle::getPointByAngle(const double angle) const noexcept
	{
		const double s = std::sin(angle);
		const double c = std::cos(angle);
		return{ ((s * r) + x), ((-c * r) + y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtLength
	//
	////////////////////////////////////////////////////////////////

	Circle::position_type Circle::pointAtLength(const double length) const noexcept
	{
		return getPointByAngle(length / r);
	}

	////////////////////////////////////////////////////////////////
	//
	//	interpolatedPointAt
	//
	////////////////////////////////////////////////////////////////

	Circle::position_type Circle::interpolatedPointAt(const double t) const noexcept
	{
		return getPointByAngle(t * Math::TwoPi);
	}

	////////////////////////////////////////////////////////////////
	//
	//	outer
	//
	////////////////////////////////////////////////////////////////

	Array<Vec2> Circle::outer(const PointsPerCircle& pointsPerCircle) const
	{
		if (r == 0.0)
		{
			return{};
		}

		const uint32 n = pointsPerCircle.value();

		Array<Vec2> vertices(n, center);
		{
			Vec2* pPos = vertices.data();

			const double d = (Math::TwoPi / n);

			for (uint32 i = 0; i < n; ++i)
			{
				const auto [s, c] = FastMath::SinCos(i * d);

				(pPos++)->moveBy((s * r), (-c * r));
			}
		}

		return vertices;
	}

	Array<Vec2> Circle::outer(const QualityFactor& qualityFactor) const
	{
		return outer(qualityFactor.toPointsPerCircle(r));
	}

	////////////////////////////////////////////////////////////////
	//
	//	asPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon Circle::asPolygon(const PointsPerCircle& pointsPerCircle) const
	{
		if (r == 0.0)
		{
			return{};
		}

		const Array<Vec2> vertices = outer(pointsPerCircle);

		const uint32 n = pointsPerCircle.value();

		Array<TriangleIndex> indices(n - 2);
		{
			TriangleIndex* pIndex = indices.data();

			for (Vertex2D::IndexType i = 0; i < (n - 2); ++i)
			{
				pIndex->i0 = 0;
				pIndex->i1 = (i + 1);
				pIndex->i2 = (i + 2);
				++pIndex;
			}
		}

		return Polygon{ vertices, std::move(indices), boundingRect(), SkipValidation::Yes };
	}

	Polygon Circle::asPolygon(const QualityFactor& qualityFactor) const
	{
		return asPolygon(qualityFactor.toPointsPerCircle(r));
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftClicked, leftPressed, leftReleased
	//
	////////////////////////////////////////////////////////////////

	bool Circle::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool Circle::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool Circle::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool Circle::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool Circle::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool Circle::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool Circle::mouseOver() const noexcept
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Circle& Circle::draw(const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addCircle(
			center,
			Abs(static_cast<float>(r)),
			color0,
			color0,
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Circle& Circle::draw(const ColorF& innerColor, const ColorF& outerColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircle(
			center,
			Abs(static_cast<float>(r)),
			innerColor.toFloat4(),
			outerColor.toFloat4(),
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Circle& Circle::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircle(
			center,
			Abs(static_cast<float>(r)),
			topColor->toFloat4(),
			bottomColor->toFloat4(),
			ColorFillDirection::TopBottom
		);

		return *this;
	}

	const Circle& Circle::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircle(
			center,
			Abs(static_cast<float>(r)),
			leftColor->toFloat4(),
			rightColor->toFloat4(),
			ColorFillDirection::LeftRight
		);

		return *this;
	}

	const Circle& Circle::draw(const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircle(
			center,
			Abs(static_cast<float>(r)),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrame
	//
	////////////////////////////////////////////////////////////////

	const Circle& Circle::drawFrame(const double thickness, const ColorF& color) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), color);
	}

	const Circle& Circle::drawFrame(const double thickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), innerColor, outerColor);
	}

	const Circle& Circle::drawFrame(const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addCircleFrame(
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(innerThickness + outerThickness),
			color0,
			color0
		);

		return *this;
	}

	const Circle& Circle::drawFrame(const double innerThickness, const double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircleFrame(
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(innerThickness + outerThickness),
			innerColor.toFloat4(),
			outerColor.toFloat4()
		);

		return *this;
	}

	const Circle& Circle::drawFrame(const double thickness, const PatternParameters& pattern) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), pattern);
	}

	const Circle& Circle::drawFrame(const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircleFrame(
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(innerThickness + outerThickness),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawPie
	//
	////////////////////////////////////////////////////////////////

	const Circle& Circle::drawPie(const double startAngle, const double angle, const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addCirclePie(
			center,
			Abs(static_cast<float>(r)),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			color0,
			color0
		);

		return *this;
	}

	const Circle& Circle::drawPie(const double startAngle, const double angle, const ColorF& innerColor, const ColorF& outerColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addCirclePie(
			center,
			Abs(static_cast<float>(r)),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			innerColor.toFloat4(),
			outerColor.toFloat4()
		);

		return *this;
	}

	const Circle& Circle::drawPie(const double startAngle, const double angle, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addCirclePie(
			center,
			Abs(static_cast<float>(r)),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawArc
	//
	////////////////////////////////////////////////////////////////

	const Circle& Circle::drawArc(const double startAngle, const double angle, const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addCircleArc(
			LineCap::Flat,
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			static_cast<float>(innerThickness + outerThickness),
			color0,
			color0,
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Circle& Circle::drawArc(const double startAngle, const double angle, const double innerThickness, const double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircleArc(
			LineCap::Flat,
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			static_cast<float>(innerThickness + outerThickness),
			innerColor.toFloat4(),
			outerColor.toFloat4(),
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Circle& Circle::drawArc(const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Arg::start_<ColorF> startColor, const Arg::end_<ColorF> endColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircleArc(
			LineCap::Flat,
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			static_cast<float>(innerThickness + outerThickness),
			startColor->toFloat4(),
			endColor->toFloat4(),
			ColorFillDirection::LeftRight
		);

		return *this;
	}

	const Circle& Circle::drawArc(const LineCap lineCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addCircleArc(
			lineCap,
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			static_cast<float>(innerThickness + outerThickness),
			color0,
			color0,
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Circle& Circle::drawArc(const LineCap lineCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircleArc(
			lineCap,
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			static_cast<float>(innerThickness + outerThickness),
			innerColor.toFloat4(),
			outerColor.toFloat4(),
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Circle& Circle::drawArc(const LineCap lineCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Arg::start_<ColorF> startColor, const Arg::end_<ColorF> endColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircleArc(
			lineCap,
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			static_cast<float>(innerThickness + outerThickness),
			startColor->toFloat4(),
			endColor->toFloat4(),
			ColorFillDirection::LeftRight
		);

		return *this;
	}

	const Circle& Circle::drawArc(const double startAngle, const double angle, const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircleArc(
			LineCap::Flat,
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			static_cast<float>(innerThickness + outerThickness),
			pattern
		);

		return *this;
	}

	const Circle& Circle::drawArc(const LineCap lineCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addCircleArc(
			lineCap,
			center,
			static_cast<float>(Abs(r) - innerThickness),
			static_cast<float>(startAngle),
			ClampAngle(angle),
			static_cast<float>(innerThickness + outerThickness),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawSegment
	//
	////////////////////////////////////////////////////////////////

	const Circle& Circle::drawSegment(const double arcMidpointAngle, const double height, const ColorF& color) const
	{
		if ((height <= 0.0) || (r <= 0.0))
		{
			return *this;
		}

		if ((r * 2.0) <= height)
		{
			return draw(color);
		}

		const float arcAngleHalf = std::acos(1.0f - (static_cast<float>(height) / static_cast<float>(r)));

		SIV3D_ENGINE(Renderer2D)->addCircleSegment(
			center,
			Abs(static_cast<float>(r)),
			static_cast<float>(arcMidpointAngle - arcAngleHalf),
			static_cast<float>(arcAngleHalf * 2.0f),
			color.toFloat4()
		);

		return *this;
	}

	const Circle& Circle::drawSegment(const double arcMidpointAngle, const double height, const PatternParameters& pattern) const
	{
		if ((height <= 0.0) || (r <= 0.0))
		{
			return *this;
		}

		if ((r * 2.0) <= height)
		{
			return draw(pattern);
		}

		const float arcAngleHalf = std::acos(1.0f - (static_cast<float>(height) / static_cast<float>(r)));

		SIV3D_ENGINE(Renderer2D)->addCircleSegment(
			center,
			Abs(static_cast<float>(r)),
			static_cast<float>(arcMidpointAngle - arcAngleHalf),
			static_cast<float>(arcAngleHalf * 2.0f),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawSegmentFromAngles
	//
	////////////////////////////////////////////////////////////////

	const Circle& Circle::drawSegmentFromAngles(const double startAngle, const double angle, const ColorF& color) const
	{
		if (angle == 0.0)
		{
			return *this;
		}

		// angle が 2π 以上の場合は円全体を描画する
		if (Math::TwoPi <= Abs(angle))
		{
			return draw(color);
		}

		SIV3D_ENGINE(Renderer2D)->addCircleSegment(
			center,
			Abs(static_cast<float>(r)),
			static_cast<float>(startAngle),
			static_cast<float>(angle),
			color.toFloat4()
		);

		return *this;
	}

	const Circle& Circle::drawSegmentFromAngles(const double startAngle, const double angle, const PatternParameters& pattern) const
	{
		if (angle == 0.0)
		{
			return *this;
		}

		// angle が 2π 以上の場合は円全体を描画する
		if (Math::TwoPi <= Abs(angle))
		{
			return draw(pattern);
		}

		SIV3D_ENGINE(Renderer2D)->addCircleSegment(
			center,
			Abs(static_cast<float>(r)),
			static_cast<float>(startAngle),
			static_cast<float>(angle),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromPoints
	//
	////////////////////////////////////////////////////////////////

	Circle Circle::FromPoints(const position_type& p0, const position_type& p1, const position_type& p2) noexcept
	{
		return Circle{ p0, p1, p2 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Circle& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.r);
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Circle>::format(const s3d::Circle& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {}, {})", value.x, value.y, value.r);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.x, value.y, value.r));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Circle, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Circle, s3d::char32>::format(const s3d::Circle& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {}, {})", value.x, value.y, value.r);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"}), {:" + tag + U"})");
		return format_to(ctx.out(), format, value.x, value.y, value.r);
	}
}

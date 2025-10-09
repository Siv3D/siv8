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

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/LineCap.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	double SuperEllipse::area() const noexcept
	{
		if (n <= 0)
		{
			return 0.0;
		}

		const long double _a = Abs(axes.x);
		const long double _b = Abs(axes.y);

		const long double x1 = (1.0L + 1.0L / n);
		const long double x2 = (1.0L + 2.0L / n);
		const long double lnA = (std::logl(4.0L * _a * _b) + 2.0L * std::lgammal(x1) - std::lgammal(x2));
		return static_cast<double>(std::expl(lnA));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPointByAngle
	//
	////////////////////////////////////////////////////////////////

	SuperEllipse::position_type SuperEllipse::getPointByAngle(const double angle) const noexcept
	{
		if (n <= 0.0)
		{
			return center;
		}

		const long double ax = Abs(axes.x);
		const long double ay = Abs(axes.y);

		if ((ax == 0.0L) || (ay == 0.0L))
		{
			return { center.x, center.y };
		}

		const long double s = std::sin(angle);
		const long double c = std::cos(angle);
		const long double p = (2.0L / static_cast<long double>(n));
		const auto SignedPow = [p](long double v) -> long double
		{
			if (v == 0.0L)
			{
				return 0.0L;
			}

			return std::copysignl(std::powl(Abs(v), p), v);
		};

		const long double xs = SignedPow(s);
		const long double yc = -SignedPow(c);
		return{ (center.x + ax * xs), (center.y + ay * yc) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	outer
	//
	////////////////////////////////////////////////////////////////

	Array<Vec2> SuperEllipse::outer(const PointsPerCircle& pointsPerCircle) const
	{
		if (n <= 0.0)
		{
			return{};
		}

		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return{};
		}

		const uint32 count = (pointsPerCircle.value() * ((n < 1.0) ? 2u : 1u));

		Array<Vec2> vertices(count, center);
		{
			Vec2* pPos = vertices.data();

			const double axesX = Abs(axes.x);
			const double axesY = Abs(axes.y);
			const double d = (Math::TwoPi / count);
			const double p = (2.0 / n);
			const auto SignedPow = [p](double v) -> double
			{
				if (v == 0.0)
				{
					return 0.0;
				}

				return std::copysign(std::pow(Abs(v), p), v);
			};

			for (uint32 i = 0; i < count; ++i)
			{
				const auto [s, c] = FastMath::SinCos(i * d);
				const double xs = SignedPow(s);
				const double yc = -SignedPow(c);
				(pPos++)->moveBy((xs * axesX), (yc * axesY));
			}
		}

		return vertices;
	}

	Array<Vec2> SuperEllipse::outer(const QualityFactor& qualityFactor) const
	{
		const double r = Max(Abs(axes.x), Abs(axes.y));

		return outer(qualityFactor.toPointsPerCircle(r));
	}

	////////////////////////////////////////////////////////////////
	//
	//	asPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon SuperEllipse::asPolygon(const PointsPerCircle& pointsPerCircle) const
	{
		if (n <= 0.0)
		{
			return{};
		}

		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return{};
		}

		if (n < 1.0)
		{
			return Polygon{ outer(pointsPerCircle), boundingRect(), SkipValidation::No };
		}
		else if (n == 1.0)
		{
			return Quad{ left(), top(), right(), bottom() }.asPolygon();
		}
		else
		{
			const Array<Vec2> vertices = outer(pointsPerCircle);
			const size_t count = vertices.size();

			Array<TriangleIndex> indices(count - 2);
			{
				TriangleIndex* pIndex = indices.data();

				for (Vertex2D::IndexType i = 0; i < (count - 2); ++i)
				{
					pIndex->i0 = 0;
					pIndex->i1 = (i + 1);
					pIndex->i2 = (i + 2);
					++pIndex;
				}
			}

			return Polygon{ vertices, std::move(indices), boundingRect(), SkipValidation::Yes };
		}
	}

	Polygon SuperEllipse::asPolygon(const QualityFactor& qualityFactor) const
	{
		const double r = Max(Abs(axes.x), Abs(axes.y));

		return asPolygon(qualityFactor.toPointsPerCircle(r));
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftClicked, leftPressed, leftReleased
	//
	////////////////////////////////////////////////////////////////

	bool SuperEllipse::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool SuperEllipse::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool SuperEllipse::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool SuperEllipse::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool SuperEllipse::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool SuperEllipse::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool SuperEllipse::mouseOver() const noexcept
	{
		return Geometry2D::Intersect(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const SuperEllipse& SuperEllipse::draw(const ColorF& color) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0) || (n <= 0.0))
		{
			return *this;
		}

		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addSuperEllipse(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			static_cast<float>(n),
			color0,
			color0,
			ColorFillDirection::InOut
		);

		return *this;
	}

	const SuperEllipse& SuperEllipse::draw(const ColorF& innerColor, const ColorF& outerColor) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0) || (n <= 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addSuperEllipse(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			static_cast<float>(n),
			innerColor.toFloat4(),
			outerColor.toFloat4(),
			ColorFillDirection::InOut
		);

		return *this;
	}

	const SuperEllipse& SuperEllipse::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0) || (n <= 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addSuperEllipse(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			static_cast<float>(n),
			topColor->toFloat4(),
			bottomColor->toFloat4(),
			ColorFillDirection::TopBottom
		);

		return *this;
	}

	const SuperEllipse& SuperEllipse::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0) || (n <= 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addSuperEllipse(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			static_cast<float>(n),
			leftColor->toFloat4(),
			rightColor->toFloat4(),
			ColorFillDirection::LeftRight
		);

		return *this;
	}

	const SuperEllipse& SuperEllipse::draw(const PatternParameters& pattern) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0) || (n <= 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addSuperEllipse(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			static_cast<float>(n),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrame
	//
	////////////////////////////////////////////////////////////////

	const SuperEllipse& SuperEllipse::drawFrame(const double thickness, const ColorF& color) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0) || (n <= 0.0) || (thickness <= 0.0))
		{
			return *this;
		}

		const double r = (Max(Abs(axes.x), Abs(axes.y)) + Abs(thickness * 0.5));

		SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
			outer(QualityFactor{ 0.5 }.toPointsPerCircle(r)), none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::Yes,
			color.toFloat4());

		return *this;
	}

	const SuperEllipse& SuperEllipse::drawFrame(const double thickness, const PatternParameters& pattern) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0) || (n <= 0.0) || (thickness <= 0.0))
		{
			return *this;
		}

		const double r = (Max(Abs(axes.x), Abs(axes.y)) + Abs(thickness * 0.5));

		SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
			outer(QualityFactor{ 0.5 }.toPointsPerCircle(r)), none,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::Yes,
			pattern);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const SuperEllipse& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.center.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.center.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.axes.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.axes.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.n);
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::SuperEllipse>::format(const s3d::SuperEllipse& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {}, {}, {}, {})", value.center.x, value.center.y, value.axes.x, value.axes.y, value.n);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.center.x, value.center.y, value.axes.x, value.axes.y, value.n));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::SuperEllipse, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::SuperEllipse, s3d::char32>::format(const s3d::SuperEllipse& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {}, {}, {}, {})", value.center.x, value.center.y, value.axes.x, value.axes.y, value.n);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"}), {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.center.x, value.center.y, value.axes.x, value.axes.y, value.n);
	}
}

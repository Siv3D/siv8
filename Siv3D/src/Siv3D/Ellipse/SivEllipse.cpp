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

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/Utility.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/ImageDraw.hpp>
# include "../ImageDraw/ShapeExtensions.hpp"
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static std::pair<float, float> ToEllipseParametricAngles(const float rx, const float ry, const float startAngle, const float angle) noexcept
		{
			auto AngleToParam = [&](float theta)
			{
				const auto [s, c] = FastMath::SinCos(theta);
				return std::atan2((ry * s), (rx * c));
			};

			const float t0 = AngleToParam(startAngle);
			const float t1 = AngleToParam(startAngle + angle);

			float dt = (t1 - t0);

			// [-pi, +pi] に畳む
			if (dt > Math::PiF)
			{
				dt -= Math::TwoPiF;
			}
			else if (dt < -Math::PiF)
			{
				dt += Math::TwoPiF;
			}

			// sweep の符号を入力 angle に合わせる
			if ((angle > 0.0f) && (dt < 0.0f))
			{
				dt += Math::TwoPiF;
			}
			
			if ((angle < 0.0f) && (dt > 0.0f))
			{
				dt -= Math::TwoPiF;
			}

			return { t0, dt };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPointByAngle
	//
	////////////////////////////////////////////////////////////////

	Ellipse::position_type Ellipse::getPointByAngle(const double angle) const noexcept
	{
		const double s = std::sin(angle);
		const double c = std::cos(angle);
		return{ ((s * axes.x) + center.x), ((-c * axes.y) + center.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	outer
	//
	////////////////////////////////////////////////////////////////

	Array<Vec2> Ellipse::outer(const PointsPerCircle& pointsPerCircle) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
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

				(pPos++)->moveBy((s * axes.x), (-c * axes.y));
			}
		}

		return vertices;
	}

	Array<Vec2> Ellipse::outer(const QualityFactor& qualityFactor) const
	{
		const double r = Max(Abs(axes.x), Abs(axes.y));

		return outer(qualityFactor.toPointsPerCircle(r));
	}

	////////////////////////////////////////////////////////////////
	//
	//	asPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon Ellipse::asPolygon(const PointsPerCircle& pointsPerCircle) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
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

	Polygon Ellipse::asPolygon(const QualityFactor& qualityFactor) const
	{
		const double r = Max(Abs(axes.x), Abs(axes.y));

		return asPolygon(qualityFactor.toPointsPerCircle(r));
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftClicked, leftPressed, leftReleased
	//
	////////////////////////////////////////////////////////////////

	bool Ellipse::leftClicked() const noexcept
	{
		return (MouseL.down() && mouseOver());
	}

	bool Ellipse::leftPressed() const noexcept
	{
		return (MouseL.pressed() && mouseOver());
	}

	bool Ellipse::leftReleased() const noexcept
	{
		return (MouseL.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	rightClicked, rightPressed, rightReleased
	//
	////////////////////////////////////////////////////////////////

	bool Ellipse::rightClicked() const noexcept
	{
		return (MouseR.down() && mouseOver());
	}

	bool Ellipse::rightPressed() const noexcept
	{
		return (MouseR.pressed() && mouseOver());
	}

	bool Ellipse::rightReleased() const noexcept
	{
		return (MouseR.up() && mouseOver());
	}

	////////////////////////////////////////////////////////////////
	//
	//	mouseOver
	//
	////////////////////////////////////////////////////////////////

	bool Ellipse::mouseOver() const noexcept
	{
		return Geometry2D::Intersects(Cursor::PosF(), *this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	paint
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::paint(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::Fill(dst, *this, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwrite
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::overwrite(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::Fill(dst, *this, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paintFrame
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::paintFrame(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paintFrame(dst, (thickness * 0.5), (thickness * 0.5), color, enableAntialiasing);
	}

	const Ellipse& Ellipse::paintFrame(Image& dst, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::EllipseFrame(dst, *this, innerThickness, outerThickness, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwriteFrame
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::overwriteFrame(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwriteFrame(dst, (thickness * 0.5), (thickness * 0.5), color, enableAntialiasing);
	}

	const Ellipse& Ellipse::overwriteFrame(Image& dst, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::EllipseFrame(dst, *this, innerThickness, outerThickness, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paintPie
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::paintPie(Image& dst, const double startAngle, const double angle, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::EllipsePie(dst, *this, startAngle, angle, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwritePie
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::overwritePie(Image& dst, const double startAngle, const double angle, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		ImageDraw::EllipsePie(dst, *this, startAngle, angle, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paintRotated
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::paintRotated(Image& dst, const double rotation, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		detail::ImageShape::EllipseRotated(dst, *this, rotation, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwriteRotated
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::overwriteRotated(Image& dst, const double rotation, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		detail::ImageShape::EllipseRotated(dst, *this, rotation, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paintFrameRotated
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::paintFrameRotated(Image& dst, const double rotation, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paintFrameRotated(dst, rotation, thickness * 0.5, thickness * 0.5, color, enableAntialiasing);
	}

	const Ellipse& Ellipse::paintFrameRotated(Image& dst, const double rotation, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		detail::ImageShape::EllipseFrameRotated(dst, *this, rotation, innerThickness, outerThickness, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwriteFrameRotated
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::overwriteFrameRotated(Image& dst, const double rotation, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwriteFrameRotated(dst, rotation, thickness * 0.5, thickness * 0.5, color, enableAntialiasing);
	}

	const Ellipse& Ellipse::overwriteFrameRotated(Image& dst, const double rotation, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		detail::ImageShape::EllipseFrameRotated(dst, *this, rotation, innerThickness, outerThickness, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paintArc
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::paintArc(Image& dst, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paintArc(dst, LineCap::Flat, LineCap::Flat, startAngle, angle, innerThickness, outerThickness, color, enableAntialiasing);
	}

	const Ellipse& Ellipse::paintArc(Image& dst, const LineCap lineCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paintArc(dst, lineCap, lineCap, startAngle, angle, innerThickness, outerThickness, color, enableAntialiasing);
	}

	const Ellipse& Ellipse::paintArc(Image& dst, const LineCap startCap, const LineCap endCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		detail::ImageShape::EllipseArc(dst, *this, startCap, endCap, startAngle, angle, innerThickness, outerThickness, color, ImagePixel::BlendMode::SourceOver, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwriteArc
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::overwriteArc(Image& dst, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwriteArc(dst, LineCap::Flat, LineCap::Flat, startAngle, angle, innerThickness, outerThickness, color, enableAntialiasing);
	}

	const Ellipse& Ellipse::overwriteArc(Image& dst, const LineCap lineCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwriteArc(dst, lineCap, lineCap, startAngle, angle, innerThickness, outerThickness, color, enableAntialiasing);
	}

	const Ellipse& Ellipse::overwriteArc(Image& dst, const LineCap startCap, const LineCap endCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		detail::ImageShape::EllipseArc(dst, *this, startCap, endCap, startAngle, angle, innerThickness, outerThickness, color, ImagePixel::BlendMode::Overwrite, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::draw(const ColorF& color) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return *this;
		}

		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addEllipse(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			color0,
			color0,
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Ellipse& Ellipse::draw(const ColorF& innerColor, const ColorF& outerColor) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipse(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			innerColor.toFloat4(),
			outerColor.toFloat4(),
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Ellipse& Ellipse::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipse(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			topColor->toFloat4(),
			bottomColor->toFloat4(),
			ColorFillDirection::TopBottom
		);

		return *this;
	}

	const Ellipse& Ellipse::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipse(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			leftColor->toFloat4(),
			rightColor->toFloat4(),
			ColorFillDirection::LeftRight
		);

		return *this;
	}

	const Ellipse& Ellipse::draw(const PatternParameters& pattern) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipse(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrame
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::drawFrame(const double thickness, const ColorF& color) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), color);
	}

	const Ellipse& Ellipse::drawFrame(const double thickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), innerColor, outerColor);
	}

	const Ellipse& Ellipse::drawFrame(const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		const float thickness = static_cast<float>(innerThickness + outerThickness);

		if ((axes.x == 0.0) || (axes.y == 0.0) || (thickness <= 0.0))
		{
			return *this;
		}

		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addEllipseFrame(
			center,
			static_cast<float>(Abs(axes.x)),
			static_cast<float>(Abs(axes.y)),
			static_cast<float>(innerThickness),
			static_cast<float>(outerThickness),
			color0,
			color0
		);

		return *this;
	}

	const Ellipse& Ellipse::drawFrame(const double innerThickness, const double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		const float thickness = static_cast<float>(innerThickness + outerThickness);

		if ((axes.x == 0.0) || (axes.y == 0.0) || (thickness <= 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipseFrame(
			center,
			static_cast<float>(Abs(axes.x)),
			static_cast<float>(Abs(axes.y)),
			static_cast<float>(innerThickness),
			static_cast<float>(outerThickness),
			innerColor.toFloat4(),
			outerColor.toFloat4()
		);

		return *this;
	}

	const Ellipse& Ellipse::drawFrame(const double thickness, const PatternParameters& pattern) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), pattern);
	}

	const Ellipse& Ellipse::drawFrame(const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		const float thickness = static_cast<float>(innerThickness + outerThickness);

		if ((axes.x == 0.0) || (axes.y == 0.0) || (thickness <= 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipseFrame(
			center,
			static_cast<float>(Abs(axes.x)),
			static_cast<float>(Abs(axes.y)),
			static_cast<float>(innerThickness),
			static_cast<float>(outerThickness),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawRotated
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::drawRotated(const double rotation, const ColorF& color) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return *this;
		}

		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addEllipseRotated(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			static_cast<float>(rotation),
			color0,
			color0,
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Ellipse& Ellipse::drawRotated(const double rotation, const ColorF& innerColor, const ColorF& outerColor) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipseRotated(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			static_cast<float>(rotation),
			innerColor.toFloat4(),
			outerColor.toFloat4(),
			ColorFillDirection::InOut
		);

		return *this;
	}

	const Ellipse& Ellipse::drawRotated(const double rotation, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipseRotated(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			static_cast<float>(rotation),
			topColor->toFloat4(),
			bottomColor->toFloat4(),
			ColorFillDirection::TopBottom
		);

		return *this;
	}

	const Ellipse& Ellipse::drawRotated(const double rotation, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipseRotated(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			static_cast<float>(rotation),
			leftColor->toFloat4(),
			rightColor->toFloat4(),
			ColorFillDirection::LeftRight
		);

		return *this;
	}

	const Ellipse& Ellipse::drawRotated(const double rotation, const PatternParameters& pattern) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipseRotated(
			center,
			Abs(static_cast<float>(axes.x)),
			Abs(static_cast<float>(axes.y)),
			static_cast<float>(rotation),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrameRotated
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::drawFrameRotated(const double rotation, const double thickness, const ColorF& color) const
	{
		return drawFrameRotated(rotation, (thickness * 0.5), (thickness * 0.5), color);
	}

	const Ellipse& Ellipse::drawFrameRotated(const double rotation, const double thickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		return drawFrameRotated(rotation, (thickness * 0.5), (thickness * 0.5), innerColor, outerColor);
	}

	const Ellipse& Ellipse::drawFrameRotated(const double rotation, const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		const float thickness = static_cast<float>(innerThickness + outerThickness);

		if ((axes.x == 0.0) || (axes.y == 0.0) || (thickness <= 0.0))
		{
			return *this;
		}

		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addEllipseFrameRotated(
			center,
			static_cast<float>(Abs(axes.x)),
			static_cast<float>(Abs(axes.y)),
			static_cast<float>(rotation),
			static_cast<float>(innerThickness),
			static_cast<float>(outerThickness),
			color0,
			color0
		);

		return *this;
	}

	const Ellipse& Ellipse::drawFrameRotated(const double rotation, const double innerThickness, const double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		const float thickness = static_cast<float>(innerThickness + outerThickness);

		if ((axes.x == 0.0) || (axes.y == 0.0) || (thickness <= 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipseFrameRotated(
			center,
			static_cast<float>(Abs(axes.x)),
			static_cast<float>(Abs(axes.y)),
			static_cast<float>(rotation),
			static_cast<float>(innerThickness),
			static_cast<float>(outerThickness),
			innerColor.toFloat4(),
			outerColor.toFloat4()
		);

		return *this;
	}

	const Ellipse& Ellipse::drawFrameRotated(const double rotation, const double thickness, const PatternParameters& pattern) const
	{
		return drawFrameRotated(rotation, (thickness * 0.5), (thickness * 0.5), pattern);
	}

	const Ellipse& Ellipse::drawFrameRotated(const double rotation, const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		const float thickness = static_cast<float>(innerThickness + outerThickness);

		if ((axes.x == 0.0) || (axes.y == 0.0) || (thickness <= 0.0))
		{
			return *this;
		}

		SIV3D_ENGINE(Renderer2D)->addEllipseFrameRotated(
			center,
			static_cast<float>(Abs(axes.x)),
			static_cast<float>(Abs(axes.y)),
			static_cast<float>(rotation),
			static_cast<float>(innerThickness),
			static_cast<float>(outerThickness),
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawDashedFrame
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::drawDashedFrame(const double thickness, const RectangularDashStyle& style, const ColorF& color) const
	{
		return drawDashedFrame((thickness * 0.5), (thickness * 0.5), style, color);
	}

	const Ellipse& Ellipse::drawDashedFrame(const double thickness, const RectangularDashStyle& style, const ColorF& innerColor, const ColorF& outerColor) const
	{
		return drawDashedFrame((thickness * 0.5), (thickness * 0.5), style, innerColor, outerColor);
	}

	const Ellipse& Ellipse::drawDashedFrame(const double innerThickness, const double outerThickness, const RectangularDashStyle& style, const ColorF& color) const
	{
		return drawDashedFrame(innerThickness, outerThickness, style, color, color);
	}

	const Ellipse& Ellipse::drawDashedFrame(const double innerThickness, const double outerThickness, const RectangularDashStyle& style, const ColorF& innerColor, const ColorF& outerColor) const
	{
		const float thickness = static_cast<float>(innerThickness + outerThickness);
		
		if ((axes.x == 0.0) || (axes.y == 0.0) || (thickness <= 0.0))
		{
			return *this;
		}
		
		SIV3D_ENGINE(Renderer2D)->addEllipseDashedFrame(
			center,
			static_cast<float>(Abs(axes.x)),
			static_cast<float>(Abs(axes.y)),
			static_cast<float>(innerThickness),
			static_cast<float>(outerThickness),
			static_cast<float>(style.offset),
			static_cast<float>(style.dashRatio),
			style.dashCount,
			innerColor.toFloat4(),
			outerColor.toFloat4()
		);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawArc
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::drawArc(const double startAngle, const double angle, const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		return drawArc(LineCap::Flat, LineCap::Flat, startAngle, angle, innerThickness, outerThickness, color);
	}

	const Ellipse& Ellipse::drawArc(const double startAngle, const double angle, const double innerThickness, const double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		return drawArc(LineCap::Flat, LineCap::Flat, startAngle, angle, innerThickness, outerThickness, innerColor, outerColor);
	}

	const Ellipse& Ellipse::drawArc(const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Arg::start_<ColorF> startColor, const Arg::end_<ColorF> endColor) const
	{
		return drawArc(LineCap::Flat, LineCap::Flat, startAngle, angle, innerThickness, outerThickness, startColor, endColor);
	}

	const Ellipse& Ellipse::drawArc(const double startAngle, const double angle, const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		return drawArc(LineCap::Flat, LineCap::Flat, startAngle, angle, innerThickness, outerThickness, pattern);
	}

	const Ellipse& Ellipse::drawArc(const LineCap lineCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		return drawArc(lineCap, lineCap, startAngle, angle, innerThickness, outerThickness, color);
	}

	const Ellipse& Ellipse::drawArc(const LineCap lineCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		return drawArc(lineCap, lineCap, startAngle, angle, innerThickness, outerThickness, innerColor, outerColor);
	}

	const Ellipse& Ellipse::drawArc(const LineCap lineCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Arg::start_<ColorF> startColor, const Arg::end_<ColorF> endColor) const
	{
		return drawArc(lineCap, lineCap, startAngle, angle, innerThickness, outerThickness, startColor, endColor);
	}

	const Ellipse& Ellipse::drawArc(const LineCap lineCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		return drawArc(lineCap, lineCap, startAngle, angle, innerThickness, outerThickness, pattern);
	}

	const Ellipse& Ellipse::drawArc(const LineCap startCap, const LineCap endCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		return drawArc(startCap, endCap, startAngle, angle, innerThickness, outerThickness, color, color);
	}

	const Ellipse& Ellipse::drawArc(const LineCap startCap, const LineCap endCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const ColorF& innerColor, const ColorF& outerColor) const
	{
		if ((a == 0.0) || (b == 0.0) || ((innerThickness + outerThickness) <= 0.0)) return *this;
		const float sweep = Clamp(static_cast<float>(angle), -Math::TwoPiF, Math::TwoPiF);
		if (Abs(sweep) >= Math::TwoPiF) return drawFrame(innerThickness, outerThickness, innerColor, outerColor);
		const float rx = Abs(static_cast<float>(a)), ry = Abs(static_cast<float>(b));
		auto [start, span] = ToEllipseParametricAngles(rx, ry, static_cast<float>(startAngle), sweep);
		SIV3D_ENGINE(Renderer2D)->addEllipseArc(startCap, endCap, center, rx, ry, start, span,
			static_cast<float>(innerThickness), static_cast<float>(outerThickness), innerColor.toFloat4(), outerColor.toFloat4(), ColorFillDirection::InOut);
		return *this;
	}

	const Ellipse& Ellipse::drawArc(const LineCap startCap, const LineCap endCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const Arg::start_<ColorF> startColor, const Arg::end_<ColorF> endColor) const
	{
		if ((a == 0.0) || (b == 0.0) || ((innerThickness + outerThickness) <= 0.0)) return *this;
		const float sweep = Clamp(static_cast<float>(angle), -Math::TwoPiF, Math::TwoPiF);
		const float rx = Abs(static_cast<float>(a)), ry = Abs(static_cast<float>(b));
		auto [start, span] = ToEllipseParametricAngles(rx, ry, static_cast<float>(startAngle), sweep);
		if (Abs(sweep) >= Math::TwoPiF) span = sweep;
		SIV3D_ENGINE(Renderer2D)->addEllipseArc(startCap, endCap, center, rx, ry, start, span,
			static_cast<float>(innerThickness), static_cast<float>(outerThickness), startColor->toFloat4(), endColor->toFloat4(), ColorFillDirection::LeftRight);
		return *this;
	}

	const Ellipse& Ellipse::drawArc(const LineCap startCap, const LineCap endCap, const double startAngle, const double angle, const double innerThickness, const double outerThickness, const PatternParameters& pattern) const
	{
		if ((a == 0.0) || (b == 0.0) || ((innerThickness + outerThickness) <= 0.0)) return *this;
		const float sweep = Clamp(static_cast<float>(angle), -Math::TwoPiF, Math::TwoPiF);
		if (Abs(sweep) >= Math::TwoPiF) return drawFrame(innerThickness, outerThickness, pattern);
		const float rx = Abs(static_cast<float>(a)), ry = Abs(static_cast<float>(b));
		auto [start, span] = ToEllipseParametricAngles(rx, ry, static_cast<float>(startAngle), sweep);
		SIV3D_ENGINE(Renderer2D)->addEllipseArc(startCap, endCap, center, rx, ry, start, span,
			static_cast<float>(innerThickness), static_cast<float>(outerThickness), pattern);
		return *this;
	}

	const Ellipse& Ellipse::drawArc(const LineStyle& style, const double startAngle, const double angle,
		const double innerThickness, const double outerThickness, const ColorF& color) const
	{
		if (style.type == LinePattern::Solid) return drawArc(style.cap, startAngle, angle, innerThickness, outerThickness, color);
		if ((a == 0.0) || (b == 0.0) || ((innerThickness + outerThickness) <= 0.0)) return *this;
		const float rx = Abs(static_cast<float>(a)), ry = Abs(static_cast<float>(b));
		const float sweep = Clamp(static_cast<float>(angle), -Math::TwoPiF, Math::TwoPiF);
		auto [start, span] = ToEllipseParametricAngles(rx, ry, static_cast<float>(startAngle), sweep);
		if (Abs(sweep) >= Math::TwoPiF) span = sweep;
		SIV3D_ENGINE(Renderer2D)->addArcStyled(style, center, rx, ry, start, span,
			static_cast<float>(innerThickness), static_cast<float>(outerThickness), color.toFloat4());
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawPie
	//
	////////////////////////////////////////////////////////////////

	const Ellipse& Ellipse::drawPie(const double startAngle, const double angle, const ColorF& color) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0) || (angle == 0.0))
		{
			return *this;
		}

		if (Math::TwoPiF <= Abs(static_cast<float>(angle)))
		{
			return draw(color);
		}

		const float rx = Abs(static_cast<float>(a));
		const float ry = Abs(static_cast<float>(b));
		const auto& [startAngleF, angleF] = ToEllipseParametricAngles(rx, ry, static_cast<float>(startAngle), static_cast<float>(angle));
		const Float4 color0 = color.toFloat4();

		SIV3D_ENGINE(Renderer2D)->addEllipsePie(
			center,
			rx, ry,
			startAngleF,
			angleF,
			color0,
			color0
		);

		return *this;
	}

	const Ellipse& Ellipse::drawPie(const double startAngle, const double angle, const ColorF& innerColor, const ColorF& outerColor) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0) || (angle == 0.0))
		{
			return *this;
		}

		if (Math::TwoPiF <= Abs(static_cast<float>(angle)))
		{
			return draw(innerColor, outerColor);
		}

		const float rx = Abs(static_cast<float>(a));
		const float ry = Abs(static_cast<float>(b));
		const auto& [startAngleF, angleF] = ToEllipseParametricAngles(rx, ry, static_cast<float>(startAngle), static_cast<float>(angle));

		SIV3D_ENGINE(Renderer2D)->addEllipsePie(
			center,
			rx, ry,
			startAngleF,
			angleF,
			innerColor.toFloat4(),
			outerColor.toFloat4()
		);

		return *this;
	}

	const Ellipse& Ellipse::drawPie(const double startAngle, const double angle, const PatternParameters& pattern) const
	{
		if ((axes.x == 0.0) || (axes.y == 0.0) || (angle == 0.0))
		{
			return *this;
		}

		if (Math::TwoPiF <= Abs(static_cast<float>(angle)))
		{
			return draw(pattern);
		}

		const float rx = Abs(static_cast<float>(a));
		const float ry = Abs(static_cast<float>(b));
		const auto& [startAngleF, angleF] = ToEllipseParametricAngles(rx, ry, static_cast<float>(startAngle), static_cast<float>(angle));

		SIV3D_ENGINE(Renderer2D)->addEllipsePie(
			center,
			rx, ry,
			startAngleF,
			angleF,
			pattern
		);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Ellipse& value)
	{
		formatData.string.push_back(U'(');
		detail::AppendFloat(formatData.string, value.center.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.center.y);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.axes.x);
		formatData.string.append(U", "_sv);
		detail::AppendFloat(formatData.string, value.axes.y);
		formatData.string.push_back(U')');
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Ellipse>::format(const s3d::Ellipse& value, fmt::format_context& ctx) const
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "({}, {}, {}, {})", value.center.x, value.center.y, value.axes.x, value.axes.y);
	}
	else
	{
		const std::string format
			= ("({:" + tag + "}, {:" + tag + "}, {:" + tag + "}, {:" + tag + "})");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.center.x, value.center.y, value.axes.x, value.axes.y));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Ellipse, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Ellipse, s3d::char32>::format(const s3d::Ellipse& value, s3d::BufferContext& ctx) const
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"({}, {}, {}, {})", value.center.x, value.center.y, value.axes.x, value.axes.y);
	}
	else
	{
		const std::u32string format
			= (U"({:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"}, {:" + tag + U"})");
		return format_to(ctx.out(), format, value.center.x, value.center.y, value.axes.x, value.axes.y);
	}
}

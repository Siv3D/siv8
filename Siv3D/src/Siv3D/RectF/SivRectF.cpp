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
# include <Siv3D/LineString.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/Cursor.hpp>
# include <Siv3D/TextureRegion.hpp>
# include <Siv3D/TexturedQuad.hpp>
# include <Siv3D/ImageDraw.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/LineStyle.hpp>
# include <Siv3D/QuarterArcTable.hpp>
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

		[[nodiscard]]
		constexpr Vertex2D::IndexType CaluculateFanQuality(const double r) noexcept
		{
			return r <= 1.0 ? 3
				: r <= 6.0 ? 5
				: r <= 12.0 ? 8
				: static_cast<Vertex2D::IndexType>(Min<double>(QuarterArcTable::MaxQuality, (r * 0.2 + 6)));
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
	//	rounded
	//
	////////////////////////////////////////////////////////////////

	Polygon RectF::rounded(double tl, double tr, double br, double bl) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		tl = Max(tl, 0.0);
		tr = Max(tr, 0.0);
		br = Max(br, 0.0);
		bl = Max(bl, 0.0);

		double radiusScale = 1.0;

		if (const double t = (tl + tr); w < t)
		{
			radiusScale = Min(radiusScale, (w / t));
		}

		if (const double r = (tr + br); h < r)
		{
			radiusScale = Min(radiusScale, (h / r));
		}

		if (const double b = (bl + br); w < b)
		{
			radiusScale = Min(radiusScale, (w / b));
		}

		if (const double l = (tl + bl); h < l)
		{
			radiusScale = Min(radiusScale, (h / l));
		}

		if (radiusScale < 1.0)
		{
			tl *= radiusScale;
			tr *= radiusScale;
			br *= radiusScale;
			bl *= radiusScale;
		}

		const Vec2 rectTL = this->tl();
		const Vec2 rectTR = this->tr();
		const Vec2 rectBR = this->br();
		const Vec2 rectBL = this->bl();

		if (not (tl || tr || br || bl))
		{
			return asPolygon();
		}

		const float scale = SIV3D_ENGINE(Renderer2D)->getMaxScaling();

		const Vertex2D::IndexType tlQuality = (tl ? CaluculateFanQuality(tl * scale) : 1);
		const Vertex2D::IndexType trQuality = (tr ? CaluculateFanQuality(tr * scale) : 1);
		const Vertex2D::IndexType brQuality = (br ? CaluculateFanQuality(br * scale) : 1);
		const Vertex2D::IndexType blQuality = (bl ? CaluculateFanQuality(bl * scale) : 1);

		Array<Vec2> vertices{ Arg::reserve = static_cast<size_t>(tlQuality + trQuality + brQuality + blQuality + 4) };

		if (tl)
		{
			const Vec2 center = (rectTL + Vec2{ tl, tl });
			const std::span<const Float2> sc = QuarterArcTable::GetUnitVectors(tlQuality);
			vertices << (rectTL + Vec2{ 0, tl });

			for (Vertex2D::IndexType i = 1; i < tlQuality; ++i)
			{
				const float s = sc[i].x;
				const float c = -sc[i].y;
				vertices << (center + Vec2{ (-tl * c), (-tl * s) });
			}

			vertices << (rectTL + Vec2{ tl, 0 });
		}
		else
		{
			vertices << rectTL;
		}

		if (tr)
		{
			const Vec2 center = (rectTR + Vec2{ -tr, tr });
			const Vec2 start = (rectTR + Vec2{ -tr, 0 });
			const std::span<const Float2> sc = QuarterArcTable::GetUnitVectors(trQuality);

			if (vertices.back() != start)
			{
				vertices << start;
			}

			for (Vertex2D::IndexType i = 1; i < trQuality; ++i)
			{
				const float s = sc[i].x;
				const float nc = sc[i].y;
				vertices << (center + Vec2{ (tr * s), (tr * nc) });
			}

			vertices << (rectTR + Vec2{ 0, tr });
		}
		else if (vertices.back() != rectTR)
		{
			vertices << rectTR;
		}

		if (br)
		{
			const Vec2 center = (rectBR + Vec2{ -br, -br });
			const Vec2 start = (rectBR + Vec2{ 0, -br });
			const std::span<const Float2> sc = QuarterArcTable::GetUnitVectors(brQuality);

			if (vertices.back() != start)
			{
				vertices << start;
			}

			for (Vertex2D::IndexType i = 1; i < brQuality; ++i)
			{
				const float s = sc[i].x;
				const float c = -sc[i].y;
				vertices << (center + Vec2{ (br * c), (br * s) });
			}

			vertices << (rectBR + Vec2{ -br, 0 });
		}
		else if (vertices.back() != rectBR)
		{
			vertices << rectBR;
		}

		if (bl)
		{
			const Vec2 center = (rectBL + Vec2{ bl, -bl });
			const Vec2 start = (rectBL + Vec2{ bl, 0 });
			const std::span<const Float2> sc = QuarterArcTable::GetUnitVectors(blQuality);

			if (vertices.back() != start)
			{
				vertices << start;
			}

			for (Vertex2D::IndexType i = 1; i < blQuality; ++i)
			{
				const float s = sc[i].x;
				const float c = -sc[i].y;
				vertices << (center + Vec2{ (-bl * s), (bl * c) });
			}

			const Vec2 end = (rectBL + Vec2{ 0, -bl });

			if (vertices.front() != end)
			{
				vertices << end;
			}
		}
		else if (vertices.front() != rectBL)
		{
			vertices << rectBL;
		}

		Array<TriangleIndex> indices(vertices.size() - 2);

		for (Vertex2D::IndexType i = 0; i < indices.size(); ++i)
		{
			indices[i] = { 0, static_cast<Vertex2D::IndexType>(i + 1), static_cast<Vertex2D::IndexType>(i + 2) };
		}

		return Polygon{ vertices, indices, *this, SkipValidation::Yes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	chamfered
	//
	////////////////////////////////////////////////////////////////

	Polygon RectF::chamfered(double s) const
	{
		if (s <= 0.0)
		{
			return asPolygon();
		}

		s = Min(s, (Min(w, h) * 0.5));

		if (s <= 0.0)
		{
			return asPolygon();
		}

		const bool hasHorizontalEdge = ((s * 2.0) < w);
		const bool hasVerticalEdge = ((s * 2.0) < h);

		Array<Vec2> points{ Arg::reserve = 8 };

		// 上辺
		if (hasHorizontalEdge)
		{
			points.emplace_back((x + s), y);
			points.emplace_back((x + w - s), y);
		}
		else
		{
			points.emplace_back((x + w * 0.5), y);
		}

		// 右辺
		if (hasVerticalEdge)
		{
			points.emplace_back((x + w), (y + s));
			points.emplace_back((x + w), (y + h - s));
		}
		else
		{
			points.emplace_back((x + w), (y + h * 0.5));
		}

		// 下辺
		if (hasHorizontalEdge)
		{
			points.emplace_back((x + w - s), (y + h));
			points.emplace_back((x + s), (y + h));
		}
		else
		{
			points.emplace_back((x + w * 0.5), (y + h));
		}

		// 左辺
		if (hasVerticalEdge)
		{
			points.emplace_back(x, (y + h - s));
			points.emplace_back(x, (y + s));
		}
		else
		{
			points.emplace_back(x, (y + h * 0.5));
		}

		Array<TriangleIndex> indices(points.size() - 2);

		for (Vertex2D::IndexType i = 0; i < indices.size(); ++i)
		{
			indices[i] = { 0, static_cast<Vertex2D::IndexType>(i + 1), static_cast<Vertex2D::IndexType>(i + 2) };
		}

		return Polygon{ points, indices, *this, SkipValidation::Yes };
	}

	Polygon RectF::chamfered(double tl, double tr, double br, double bl) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		tl = Max(tl, 0.0);
		tr = Max(tr, 0.0);
		br = Max(br, 0.0);
		bl = Max(bl, 0.0);

		if (double top = (tl + tr);
			w < top)
		{
			const double scale = (w / top);
			tl *= scale;
			tr *= scale;
		}

		if (double right = (tr + br);
			h < right)
		{
			const double scale = (h / right);
			tr *= scale;
			br *= scale;
		}

		if (double bottom = (br + bl);
			w < bottom)
		{
			const double scale = (w / bottom);
			br *= scale;
			bl *= scale;
		}

		if (double left = (bl + tl);
			h < left)
		{
			const double scale = (h / left);
			bl *= scale;
			tl *= scale;
		}

		Array<Vec2> points{ Arg::reserve = 8 };

		const auto AppendPoint = [&points](const Vec2& point)
		{
			if (points.empty()
				|| ((points.back() != point)
					&& (points.front() != point)))
			{
				points << point;
			}
		};

		// 左上
		if (tl)
		{
			AppendPoint(Vec2{ x, (y + tl) });
			AppendPoint(Vec2{ (x + tl), y });
		}
		else
		{
			AppendPoint(this->tl());
		}

		// 右上
		if (tr)
		{
			AppendPoint(Vec2{ (x + w - tr), y });
			AppendPoint(Vec2{ (x + w), (y + tr) });
		}
		else
		{
			AppendPoint(this->tr());
		}

		// 右下
		if (br)
		{
			AppendPoint(Vec2{ (x + w), (y + h - br) });
			AppendPoint(Vec2{ (x + w - br), (y + h) });
		}
		else
		{
			AppendPoint(this->br());
		}

		// 左下
		if (bl)
		{
			AppendPoint(Vec2{ (x + bl), (y + h) });
			AppendPoint(Vec2{ x, (y + h - bl) });
		}
		else
		{
			AppendPoint(this->bl());
		}

		Array<TriangleIndex> indices(points.size() - 2);

		for (Vertex2D::IndexType i = 0; i < indices.size(); ++i)
		{
			indices[i] = { 0, static_cast<Vertex2D::IndexType>(i + 1), static_cast<Vertex2D::IndexType>(i + 2) };
		}

		return Polygon{ points, indices, *this, SkipValidation::Yes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	outline
	//
	////////////////////////////////////////////////////////////////

	LineString RectF::outline(const CloseRing closeRing) const
	{
		if (closeRing)
		{
			return{ pos, tr(), br(), bl(), pos };
		}
		else
		{
			return{ pos, tr(), br(), bl() };
		}
	}

	LineString RectF::outline(double distanceFromOrigin, double length) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		const double perim = perimeter();

		if (length < 0.0)
		{
			distanceFromOrigin += length;
			length = -length;
		}

		distanceFromOrigin = WrapLength(distanceFromOrigin, perim);
		length = Min(length, perim);

		LineString points{ Arg::reserve = 6 };

		const auto AppendPoint = [&points](const Vec2& point)
		{
			if (points.empty() || (points.back() != point))
			{
				points << point;
			}
		};

		AppendPoint(pointAtLength(distanceFromOrigin));

		if (length == 0.0)
		{
			return points;
		}

		const double distanceToTarget = (distanceFromOrigin + length);

		const double cornerLengths[4] = {
			w,
			(w + h),
			((w * 2.0) + h),
			perim,
		};

		for (double base = 0.0; base <= distanceToTarget; base += perim)
		{
			for (size_t i = 0; i < 4; ++i)
			{
				const double d = (base + cornerLengths[i]);

				if ((distanceFromOrigin < d) && (d < distanceToTarget))
				{
					AppendPoint(pointAtIndex((i + 1) % 4));
				}
			}
		}

		AppendPoint(pointAtLength(distanceToTarget));

		return points;
	}

	////////////////////////////////////////////////////////////////
	//
	//	outer
	//
	////////////////////////////////////////////////////////////////

	Array<Vec2> RectF::outer() const
	{
		return{ tl(), tr(), br(), bl() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	ring
	//
	////////////////////////////////////////////////////////////////

	Array<Vec2> RectF::ring() const
	{
		return{ tl(), tr(), br(), bl(), tl() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	asPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon RectF::asPolygon() const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		return Polygon{ { tl(), tr(), br(), bl() },
			{ { 0, 1, 3 }, { 3, 1, 2 }},
			*this,
			SkipValidation::Yes };
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
	//	paint
	//
	////////////////////////////////////////////////////////////////

	const RectF& RectF::paint(Image& dst, const Color& color) const
	{
		ImageDraw::Fill(dst, *this, color, ImagePixel::BlendMode::SourceOver);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwrite
	//
	////////////////////////////////////////////////////////////////

	const RectF& RectF::overwrite(Image& dst, const Color& color) const
	{
		ImageDraw::Fill(dst, *this, color, ImagePixel::BlendMode::Overwrite);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paintFrame
	//
	////////////////////////////////////////////////////////////////

	const RectF& RectF::paintFrame(Image& dst, const double innerThickness, const double outerThickness, const Color& color) const
	{
		ImageDraw::RectFrame(dst, *this, innerThickness, outerThickness, color, ImagePixel::BlendMode::SourceOver);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwriteFrame
	//
	////////////////////////////////////////////////////////////////

	const RectF& RectF::overwriteFrame(Image& dst, const double innerThickness, const double outerThickness, const Color& color) const
	{
		ImageDraw::RectFrame(dst, *this, innerThickness, outerThickness, color, ImagePixel::BlendMode::Overwrite);
		return *this;
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

	const RectF& RectF::draw(const std::array<ColorF, 4>& colors) const
	{
		return draw(std::span<const ColorF, 4>{ colors });
	}

	const RectF& RectF::draw(const std::span<const ColorF, 4> colors) const
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

	const RectF& RectF::drawFrame(const double thickness, const LineType lineType, const ColorF& color) const
	{
		return drawFrame((thickness * 0.5), (thickness * 0.5), lineType, color);
	}

	const RectF& RectF::drawFrame(const double innerThickness, const double outerThickness, const LineType lineType, const ColorF& color) const
	{
		const float x0 = static_cast<float>(x - outerThickness);
		const float x1 = static_cast<float>(x + innerThickness);
		const float x2 = static_cast<float>(x + w - innerThickness);
		const float x3 = static_cast<float>(x + w + outerThickness);

		const float y0 = static_cast<float>(y - outerThickness);
		const float y1 = static_cast<float>(y + innerThickness);
		const float y2 = static_cast<float>(y + h - innerThickness);
		const float y3 = static_cast<float>(y + h + outerThickness);

		const float px1 = ((x0 + x1) * 0.5f);
		const float px2 = ((x2 + x3) * 0.5f);
		const float py1 = ((y0 + y1) * 0.5f);
		const float py2 = ((y2 + y3) * 0.5f);

		const float thickness = static_cast<float>(innerThickness + outerThickness);

		if (thickness <= 0.0f)
		{
			return *this;
		}

		const Float4 colors[2] = { color.toFloat4(), color.toFloat4() };

		if (lineType == LineType::RoundDot)
		{
			const LineStyle style = LineStyle::RoundDot;
			SIV3D_ENGINE(Renderer2D)->addLine(style, Float2{ px1, py1 }, Float2{ px2, py1 }, thickness, colors);
			SIV3D_ENGINE(Renderer2D)->addLine(style, Float2{ px2, py1 }, Float2{ px2, py2 }, thickness, colors);
			SIV3D_ENGINE(Renderer2D)->addLine(style, Float2{ px2, py2 }, Float2{ px1, py2 }, thickness, colors);
			SIV3D_ENGINE(Renderer2D)->addLine(style, Float2{ px1, py2 }, Float2{ px1, py1 }, thickness, colors);
		}
		else
		{
			const LineStyle lineStyle = LineStyle::Parameters{ lineType, LineCap::Flat };
			SIV3D_ENGINE(Renderer2D)->addLine(lineStyle, Float2{ x0, py1 }, Float2{ x3, py1 }, thickness, colors);
			SIV3D_ENGINE(Renderer2D)->addLine(lineStyle, Float2{ px2, y0 }, Float2{ px2, y3 }, thickness, colors);
			SIV3D_ENGINE(Renderer2D)->addLine(lineStyle, Float2{ x3, py2 }, Float2{ x0, py2 }, thickness, colors);
			SIV3D_ENGINE(Renderer2D)->addLine(lineStyle, Float2{ px1, y3 }, Float2{ px1, y0 }, thickness, colors);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawShadow
	//
	////////////////////////////////////////////////////////////////

	const RectF& RectF::drawShadow(const Vec2& offset, const double blur, const double spread, const ColorF& color, const bool fill) const
	{
		const RectF baseRect = movedBy(offset).stretched(spread);

		if (blur <= 0.0)
		{
			if (fill)
			{
				baseRect.draw(color);
			}

			return *this;
		}

		const double blurClamped = Min(baseRect.w, baseRect.h, blur);

		SIV3D_ENGINE(Renderer2D)->addRectShadow(FloatRect{ baseRect.x, baseRect.y, (baseRect.x + baseRect.w), (baseRect.y + baseRect.h) },
			static_cast<float>(blurClamped), color.toFloat4(), fill);

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ()
	//
	////////////////////////////////////////////////////////////////

	TexturedQuad RectF::operator ()(const Texture& texture) const
	{
		return{
			texture,
			0.0f, 0.0f, 1.0f, 1.0f,
			asQuad(),
			center()
		};
	}

	TexturedQuad RectF::operator ()(const TextureRegion& textureRegion) const
	{
		return{
			textureRegion.texture,
			textureRegion.uvRect,
			asQuad(),
			center()
		};
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

fmt::format_context::iterator fmt::formatter<s3d::RectF>::format(const s3d::RectF& value, fmt::format_context& ctx) const
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

s3d::BufferContext::iterator fmt::formatter<s3d::RectF, s3d::char32>::format(const s3d::RectF& value, s3d::BufferContext& ctx) const
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

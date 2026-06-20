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
# include <Siv3D/RectangularDashStyle.hpp>
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

		static constexpr void ScaleCornerSizes(double& tl, double& tr, double& br, double& bl, const double w, const double h) noexcept
		{
			double scale = 1.0;

			if (const double top = (tl + tr); w < top)
			{
				scale = Min(scale, (w / top));
			}

			if (const double right = (tr + br); h < right)
			{
				scale = Min(scale, (h / right));
			}

			if (const double bottom = (br + bl); w < bottom)
			{
				scale = Min(scale, (w / bottom));
			}

			if (const double left = (bl + tl); h < left)
			{
				scale = Min(scale, (h / left));
			}

			if (scale < 1.0)
			{
				tl *= scale;
				tr *= scale;
				br *= scale;
				bl *= scale;
			}
		}

		[[nodiscard]]
		static constexpr double Cross(const Vec2& a, const Vec2& b, const Vec2& c) noexcept
		{
			return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x));
		}

		static void SimplifyPolygonPoints(Array<Vec2>& points)
		{
			bool changed = true;

			while (changed && (2 < points.size()))
			{
				changed = false;

				for (size_t i = 0; i < points.size(); ++i)
				{
					const size_t next = ((i + 1) % points.size());

					if (points[i] == points[next])
					{
						points.erase(points.begin() + next);
						changed = true;
						break;
					}
				}

				if (changed)
				{
					continue;
				}

				for (size_t i = 0; i < points.size(); ++i)
				{
					const size_t prev = ((i + points.size() - 1) % points.size());
					const size_t next = ((i + 1) % points.size());

					if (points[prev] == points[next])
					{
						points.erase(points.begin() + i);
						changed = true;
						break;
					}
				}

				if (changed)
				{
					continue;
				}

				for (size_t i = 0; i < points.size(); ++i)
				{
					const Vec2& a = points[(i + points.size() - 1) % points.size()];
					const Vec2& b = points[i];
					const Vec2& c = points[(i + 1) % points.size()];
					const double dot = ((b.x - a.x) * (c.x - b.x) + (b.y - a.y) * (c.y - b.y));

					if ((Abs(Cross(a, b, c)) <= 1.0e-10) && (0.0 <= dot))
					{
						points.erase(points.begin() + i);
						changed = true;
						break;
					}
				}
			}
		}

		[[nodiscard]]
		static Array<TriangleIndex> TriangulateCCW(const Array<Vec2>& points)
		{
			if (points.size() < 3)
			{
				return{};
			}

			Array<TriangleIndex> indices{ Arg::reserve = (points.size() - 2) };
			Array<Vertex2D::IndexType> remaining{ Arg::reserve = points.size() };

			for (Vertex2D::IndexType i = 0; i < points.size(); ++i)
			{
				remaining << i;
			}

			const auto ContainsPoint = [](const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& p)
			{
				constexpr double eps = 1.0e-10;

				return ((eps < Cross(a, b, p))
					&& (eps < Cross(b, c, p))
					&& (eps < Cross(c, a, p)));
			};

			while (3 < remaining.size())
			{
				bool earFound = false;

				for (size_t i = 0; i < remaining.size(); ++i)
				{
					const Vertex2D::IndexType a = remaining[(i + remaining.size() - 1) % remaining.size()];
					const Vertex2D::IndexType b = remaining[i];
					const Vertex2D::IndexType c = remaining[(i + 1) % remaining.size()];

					if (Cross(points[a], points[b], points[c]) <= 0.0)
					{
						continue;
					}

					bool hasPoint = false;

					for (const auto index : remaining)
					{
						if ((index == a) || (index == b) || (index == c))
						{
							continue;
						}

						if (ContainsPoint(points[a], points[b], points[c], points[index]))
						{
							hasPoint = true;
							break;
						}
					}

					if (not hasPoint)
					{
						indices << TriangleIndex{ a, b, c };
						remaining.erase(remaining.begin() + i);
						earFound = true;
						break;
					}
				}

				if (not earFound)
				{
					return{};
				}
			}

			indices << TriangleIndex{ remaining[0], remaining[1], remaining[2] };

			return indices;
		}

		[[nodiscard]]
		static 	Polygon MakeRectFramePolygon(const RectF& outerRect, const RectF& innerRect)
		{
			const Vec2 outerTL = outerRect.tl();
			const Vec2 outerTR = outerRect.tr();
			const Vec2 outerBR = outerRect.br();
			const Vec2 outerBL = outerRect.bl();

			const Vec2 innerTL = innerRect.tl();
			const Vec2 innerTR = innerRect.tr();
			const Vec2 innerBR = innerRect.br();
			const Vec2 innerBL = innerRect.bl();

			Array<Vec2> outer{ Arg::reserve = 4 };
			outer << outerTL;
			outer << outerTR;
			outer << outerBR;
			outer << outerBL;

			Array<Vec2> hole{ Arg::reserve = 4 };
			hole << innerTL;
			hole << innerBL;
			hole << innerBR;
			hole << innerTR;

			Array<Array<Vec2>> holes{ Arg::reserve = 1 };
			holes << std::move(hole);

			Array<Float2> vertices(8);
			vertices[0] = outerTL;
			vertices[1] = outerTR;
			vertices[2] = outerBR;
			vertices[3] = outerBL;
			vertices[4] = innerTL;
			vertices[5] = innerTR;
			vertices[6] = innerBR;
			vertices[7] = innerBL;

			Array<TriangleIndex> indices(8);
			indices[0] = { 0, 1, 5 };
			indices[1] = { 0, 5, 4 };
			indices[2] = { 1, 2, 6 };
			indices[3] = { 1, 6, 5 };
			indices[4] = { 2, 3, 7 };
			indices[5] = { 2, 7, 6 };
			indices[6] = { 3, 0, 4 };
			indices[7] = { 3, 4, 7 };

			return Polygon{ outer, std::move(holes), std::move(vertices), std::move(indices), outerRect, SkipValidation::Yes };
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
	//	pointed
	//
	////////////////////////////////////////////////////////////////

	Polygon RectF::pointed(Arg::top_<double> topOffset) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		const double offset = Max(*topOffset, -h);

		if (offset == 0.0)
		{
			return asPolygon();
		}

		Array<Vec2> points{ Arg::reserve = 5 };
		points << this->tl();
		points << Vec2{ (x + w * 0.5), (y - offset) };
		points << this->tr();
		points << this->br();
		points << this->bl();

		Array<TriangleIndex> indices(3);

		if (0.0 < offset)
		{
			indices[0] = { 0, 1, 2 };
			indices[1] = { 0, 2, 3 };
			indices[2] = { 0, 3, 4 };
		}
		else
		{
			indices[0] = { 0, 1, 4 };
			indices[1] = { 1, 3, 4 };
			indices[2] = { 1, 2, 3 };
		}

		const RectF bounds = (0.0 < offset)
			? RectF{ x, (y - offset), w, (h + offset) }
		: *this;

		return Polygon{ points, indices, bounds, SkipValidation::Yes };
	}

	Polygon RectF::pointed(Arg::right_<double> rightOffset) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		const double offset = Max(*rightOffset, -w);

		if (offset == 0.0)
		{
			return asPolygon();
		}

		Array<Vec2> points{ Arg::reserve = 5 };
		points << this->tl();
		points << this->tr();
		points << Vec2{ (x + w + offset), (y + h * 0.5) };
		points << this->br();
		points << this->bl();

		Array<TriangleIndex> indices(3);

		if (0.0 < offset)
		{
			indices[0] = { 0, 1, 2 };
			indices[1] = { 0, 2, 3 };
			indices[2] = { 0, 3, 4 };
		}
		else
		{
			indices[0] = { 0, 1, 2 };
			indices[1] = { 2, 3, 4 };
			indices[2] = { 2, 4, 0 };
		}

		const RectF bounds = (0.0 < offset)
			? RectF{ x, y, (w + offset), h }
		: *this;

		return Polygon{ points, indices, bounds, SkipValidation::Yes };
	}

	Polygon RectF::pointed(Arg::bottom_<double> bottomOffset) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		const double offset = Max(*bottomOffset, -h);

		if (offset == 0.0)
		{
			return asPolygon();
		}

		Array<Vec2> points{ Arg::reserve = 5 };
		points << this->tl();
		points << this->tr();
		points << this->br();
		points << Vec2{ (x + w * 0.5), (y + h + offset) };
		points << this->bl();

		Array<TriangleIndex> indices(3);

		if (0.0 < offset)
		{
			indices[0] = { 0, 1, 2 };
			indices[1] = { 0, 2, 3 };
			indices[2] = { 0, 3, 4 };
		}
		else
		{
			indices[0] = { 1, 2, 3 };
			indices[1] = { 3, 4, 0 };
			indices[2] = { 3, 0, 1 };
		}

		const RectF bounds = (0.0 < offset)
			? RectF{ x, y, w, (h + offset) }
		: *this;

		return Polygon{ points, indices, bounds, SkipValidation::Yes };
	}

	Polygon RectF::pointed(Arg::left_<double> leftOffset) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		const double offset = Max(*leftOffset, -w);

		if (offset == 0.0)
		{
			return asPolygon();
		}

		Array<Vec2> points{ Arg::reserve = 5 };
		points << this->tl();
		points << this->tr();
		points << this->br();
		points << this->bl();
		points << Vec2{ (x - offset), (y + h * 0.5) };

		Array<TriangleIndex> indices(3);

		if (0.0 < offset)
		{
			indices[0] = { 0, 1, 2 };
			indices[1] = { 0, 2, 3 };
			indices[2] = { 0, 3, 4 };
		}
		else
		{
			indices[0] = { 4, 0, 1 };
			indices[1] = { 4, 1, 2 };
			indices[2] = { 2, 3, 4 };
		}

		const RectF bounds = (0.0 < offset)
			? RectF{ (x - offset), y, (w + offset), h }
		: *this;

		return Polygon{ points, indices, bounds, SkipValidation::Yes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	stepped
	//
	////////////////////////////////////////////////////////////////

	Polygon RectF::stepped(double s) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

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

		if (not hasHorizontalEdge && not hasVerticalEdge)
		{
			return{};
		}

		if (not hasHorizontalEdge)
		{
			return RectF{ x, (y + s), w, (h - s * 2.0) }.asPolygon();
		}

		if (not hasVerticalEdge)
		{
			return RectF{ (x + s), y, (w - s * 2.0), h }.asPolygon();
		}

		Array<Vec2> points{ Arg::reserve = 12 };

		points << Vec2{ (x + s), y };
		points << Vec2{ (x + w - s), y };
		points << Vec2{ (x + w - s), (y + s) };
		points << Vec2{ (x + w), (y + s) };
		points << Vec2{ (x + w), (y + h - s) };
		points << Vec2{ (x + w - s), (y + h - s) };
		points << Vec2{ (x + w - s), (y + h) };
		points << Vec2{ (x + s), (y + h) };
		points << Vec2{ (x + s), (y + h - s) };
		points << Vec2{ x, (y + h - s) };
		points << Vec2{ x, (y + s) };
		points << Vec2{ (x + s), (y + s) };

		Array<TriangleIndex> indices(10);
		indices[0] = { 0, 1, 2 };
		indices[1] = { 0, 2, 11 };
		indices[2] = { 2, 3, 4 };
		indices[3] = { 2, 4, 5 };
		indices[4] = { 11, 2, 5 };
		indices[5] = { 11, 5, 8 };
		indices[6] = { 10, 11, 8 };
		indices[7] = { 10, 8, 9 };
		indices[8] = { 8, 5, 6 };
		indices[9] = { 8, 6, 7 };

		return Polygon{ points, indices, *this, SkipValidation::Yes };
	}

	Polygon RectF::stepped(double tl, double tr, double br, double bl) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		tl = Max(tl, 0.0);
		tr = Max(tr, 0.0);
		br = Max(br, 0.0);
		bl = Max(bl, 0.0);

		if (not (tl || tr || br || bl))
		{
			return asPolygon();
		}

		ScaleCornerSizes(tl, tr, br, bl, w, h);

		Array<Vec2> points{ Arg::reserve = 12 };

		const auto AppendPoint = [&points](const Vec2& point)
			{
				if (points.empty()
					|| ((points.back() != point)
						&& (points.front() != point)))
				{
					points << point;
				}
			};

		AppendPoint(tl ? Vec2{ (x + tl), y } : this->tl());

		if (tr)
		{
			AppendPoint(Vec2{ (x + w - tr), y });
			AppendPoint(Vec2{ (x + w - tr), (y + tr) });
			AppendPoint(Vec2{ (x + w), (y + tr) });
		}
		else
		{
			AppendPoint(this->tr());
		}

		if (br)
		{
			AppendPoint(Vec2{ (x + w), (y + h - br) });
			AppendPoint(Vec2{ (x + w - br), (y + h - br) });
			AppendPoint(Vec2{ (x + w - br), (y + h) });
		}
		else
		{
			AppendPoint(this->br());
		}

		if (bl)
		{
			AppendPoint(Vec2{ (x + bl), (y + h) });
			AppendPoint(Vec2{ (x + bl), (y + h - bl) });
			AppendPoint(Vec2{ x, (y + h - bl) });
		}
		else
		{
			AppendPoint(this->bl());
		}

		if (tl)
		{
			AppendPoint(Vec2{ x, (y + tl) });
			AppendPoint(Vec2{ (x + tl), (y + tl) });
		}

		SimplifyPolygonPoints(points);

		const Array<TriangleIndex> indices = TriangulateCCW(points);

		if (indices.isEmpty())
		{
			return{};
		}

		return Polygon{ points, indices, *this, SkipValidation::Yes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scooped
	//
	////////////////////////////////////////////////////////////////

	Polygon RectF::scooped(double r) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		if (r <= 0.0)
		{
			return asPolygon();
		}

		r = Min(r, (Min(w, h) * 0.5));

		if (r <= 0.0)
		{
			return asPolygon();
		}

		const float scale = SIV3D_ENGINE(Renderer2D)->getMaxScaling();
		const Vertex2D::IndexType quality = CaluculateFanQuality(r * scale);
		const std::span<const Float2> sc = QuarterArcTable::GetUnitVectors(quality);

		Array<Vec2> points{ Arg::reserve = static_cast<size_t>(quality * 4 + 4) };

		const auto AppendPoint = [&points](const Vec2& point)
			{
				if (points.empty()
					|| ((points.back() != point)
						&& (points.front() != point)))
				{
					points << point;
				}
			};

		AppendPoint(Vec2{ (x + r), y });
		AppendPoint(Vec2{ (x + w - r), y });

		for (Vertex2D::IndexType i = 1; i < quality; ++i)
		{
			const float s = sc[i].x;
			const float c = -sc[i].y;
			AppendPoint(Vec2{ (x + w - r * c), (y + r * s) });
		}

		AppendPoint(Vec2{ (x + w), (y + r) });
		AppendPoint(Vec2{ (x + w), (y + h - r) });

		for (Vertex2D::IndexType i = 1; i < quality; ++i)
		{
			const float s = sc[i].x;
			const float c = -sc[i].y;
			AppendPoint(Vec2{ (x + w - r * s), (y + h - r * c) });
		}

		AppendPoint(Vec2{ (x + w - r), (y + h) });
		AppendPoint(Vec2{ (x + r), (y + h) });

		for (Vertex2D::IndexType i = 1; i < quality; ++i)
		{
			const float s = sc[i].x;
			const float c = -sc[i].y;
			AppendPoint(Vec2{ (x + r * c), (y + h - r * s) });
		}

		AppendPoint(Vec2{ x, (y + h - r) });
		AppendPoint(Vec2{ x, (y + r) });

		for (Vertex2D::IndexType i = 1; i < quality; ++i)
		{
			const float s = sc[i].x;
			const float c = -sc[i].y;
			AppendPoint(Vec2{ (x + r * s), (y + r * c) });
		}

		SimplifyPolygonPoints(points);

		const Array<TriangleIndex> indices = TriangulateCCW(points);

		if (indices.isEmpty())
		{
			return{};
		}

		return Polygon{ points, indices, *this, SkipValidation::Yes };
	}

	Polygon RectF::scooped(double tl, double tr, double br, double bl) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		tl = Max(tl, 0.0);
		tr = Max(tr, 0.0);
		br = Max(br, 0.0);
		bl = Max(bl, 0.0);

		if (not (tl || tr || br || bl))
		{
			return asPolygon();
		}

		ScaleCornerSizes(tl, tr, br, bl, w, h);

		const float scale = SIV3D_ENGINE(Renderer2D)->getMaxScaling();
		const Vertex2D::IndexType tlQuality = (tl ? CaluculateFanQuality(tl * scale) : 1);
		const Vertex2D::IndexType trQuality = (tr ? CaluculateFanQuality(tr * scale) : 1);
		const Vertex2D::IndexType brQuality = (br ? CaluculateFanQuality(br * scale) : 1);
		const Vertex2D::IndexType blQuality = (bl ? CaluculateFanQuality(bl * scale) : 1);

		Array<Vec2> points{ Arg::reserve = static_cast<size_t>(tlQuality + trQuality + brQuality + blQuality + 4) };

		const auto AppendPoint = [&points](const Vec2& point)
			{
				if (points.empty()
					|| ((points.back() != point)
						&& (points.front() != point)))
				{
					points << point;
				}
			};

		AppendPoint(tl ? Vec2{ (x + tl), y } : this->tl());

		if (tr)
		{
			AppendPoint(Vec2{ (x + w - tr), y });

			const std::span<const Float2> sc = QuarterArcTable::GetUnitVectors(trQuality);

			for (Vertex2D::IndexType i = 1; i < trQuality; ++i)
			{
				const float s = sc[i].x;
				const float c = -sc[i].y;
				AppendPoint(Vec2{ (x + w - tr * c), (y + tr * s) });
			}

			AppendPoint(Vec2{ (x + w), (y + tr) });
		}
		else
		{
			AppendPoint(this->tr());
		}

		if (br)
		{
			AppendPoint(Vec2{ (x + w), (y + h - br) });

			const std::span<const Float2> sc = QuarterArcTable::GetUnitVectors(brQuality);

			for (Vertex2D::IndexType i = 1; i < brQuality; ++i)
			{
				const float s = sc[i].x;
				const float c = -sc[i].y;
				AppendPoint(Vec2{ (x + w - br * s), (y + h - br * c) });
			}

			AppendPoint(Vec2{ (x + w - br), (y + h) });
		}
		else
		{
			AppendPoint(this->br());
		}

		if (bl)
		{
			AppendPoint(Vec2{ (x + bl), (y + h) });

			const std::span<const Float2> sc = QuarterArcTable::GetUnitVectors(blQuality);

			for (Vertex2D::IndexType i = 1; i < blQuality; ++i)
			{
				const float s = sc[i].x;
				const float c = -sc[i].y;
				AppendPoint(Vec2{ (x + bl * c), (y + h - bl * s) });
			}

			AppendPoint(Vec2{ x, (y + h - bl) });
		}
		else
		{
			AppendPoint(this->bl());
		}

		if (tl)
		{
			AppendPoint(Vec2{ x, (y + tl) });

			const std::span<const Float2> sc = QuarterArcTable::GetUnitVectors(tlQuality);

			for (Vertex2D::IndexType i = 1; i < tlQuality; ++i)
			{
				const float s = sc[i].x;
				const float c = -sc[i].y;
				AppendPoint(Vec2{ (x + tl * s), (y + tl * c) });
			}
		}

		SimplifyPolygonPoints(points);

		const Array<TriangleIndex> indices = TriangulateCCW(points);

		if (indices.isEmpty())
		{
			return{};
		}

		return Polygon{ points, indices, *this, SkipValidation::Yes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	frame
	//
	////////////////////////////////////////////////////////////////

	Polygon RectF::frame(double thickness) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		if (thickness <= 0.0)
		{
			return{};
		}

		const double innerThickness = Min((thickness * 0.5), (Min(w, h) * 0.5));
		const double outerThickness = (thickness * 0.5);

		if ((innerThickness <= 0.0) && (outerThickness <= 0.0))
		{
			return{};
		}

		const RectF outerRect{
			(x - outerThickness),
			(y - outerThickness),
			(w + outerThickness * 2.0),
			(h + outerThickness * 2.0)
		};

		const double innerW = (w - innerThickness * 2.0);
		const double innerH = (h - innerThickness * 2.0);

		if ((innerW <= 0.0) || (innerH <= 0.0))
		{
			return outerRect.asPolygon();
		}

		const RectF innerRect{ (x + innerThickness), (y + innerThickness), innerW, innerH };

		return MakeRectFramePolygon(outerRect, innerRect);
	}

	Polygon RectF::frame(double innerThickness, double outerThickness) const
	{
		if ((w <= 0.0) || (h <= 0.0))
		{
			return{};
		}

		innerThickness = Max(innerThickness, 0.0);
		outerThickness = Max(outerThickness, 0.0);

		if ((innerThickness <= 0.0) && (outerThickness <= 0.0))
		{
			return{};
		}

		const RectF outerRect{
			(x - outerThickness),
			(y - outerThickness),
			(w + outerThickness * 2.0),
			(h + outerThickness * 2.0)
		};

		const double innerW = (w - innerThickness * 2.0);
		const double innerH = (h - innerThickness * 2.0);

		if ((innerW <= 0.0) || (innerH <= 0.0))
		{
			return outerRect.asPolygon();
		}

		const RectF innerRect{
			(x + innerThickness),
			(y + innerThickness),
			innerW,
			innerH
		};

		return MakeRectFramePolygon(outerRect, innerRect);
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
	//	drawDashedFrame
	//
	////////////////////////////////////////////////////////////////

	const RectF& RectF::drawDashedFrame(double thickness, const RectangularDashStyle& style, const ColorF& color) const
	{
		return drawDashedFrame((thickness * 0.5), (thickness * 0.5), style, color);
	}

	const RectF& RectF::drawDashedFrame(double innerThickness, double outerThickness, const RectangularDashStyle& style, const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addRectDashedFrame(
			FloatRect{ (x + innerThickness), (y + innerThickness), (x + w - innerThickness), (y + h - innerThickness) },
			style.offset,
			static_cast<float>(innerThickness + outerThickness),
			style.dashRatio,
			style.dashCount,
			color.toFloat4());
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

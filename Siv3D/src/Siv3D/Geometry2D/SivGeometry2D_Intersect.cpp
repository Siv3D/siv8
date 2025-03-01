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
//	Authors (OpenSiv3D challenge participants)
//	- Ebishu
//	- fal_rnd
//	- きつねび
//-----------------------------------------------

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/ListUtility.hpp>
# include <Siv3D/Geometry2D/Intersect.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr bool IsZero(const double x) noexcept
		{
			return (Abs(x) < 1e-10);
		}

		struct RoundRectParts
		{
			RectF boundingRect;
			RectF rectA;
			RectF rectB;
			Circle circleTL;
			Circle circleTR;
			Circle circleBR;
			Circle circleBL;

			RoundRectParts(const RoundRect& roundRect) noexcept
			{
				const RectF& rect = roundRect.rect;
				const double rr = Min({ (rect.w * 0.5), (rect.h * 0.5), roundRect.r });
				const double x0 = rect.x;
				const double x1 = rect.x + rr;
				const double x2 = rect.x + rect.w - rr;
				const double y0 = rect.y;
				const double y1 = rect.y + rr;
				const double y2 = rect.y + rect.h - rr;
				boundingRect = roundRect.rect;
				rectA.set(x0, y1, rect.w, y2 - y1);
				rectB.set(x1, y0, x2 - x1, rect.h);
				circleTL.set(x1, y1, rr);
				circleTR.set(x2, y1, rr);
				circleBR.set(x2, y2, rr);
				circleBL.set(x1, y2, rr);
			}

			template <class Shape>
			bool intersects(const Shape& shape) const noexcept
			{
				if (not boundingRect.intersects(shape))
				{
					return false;
				}

				return (rectA.intersects(shape)
						|| rectB.intersects(shape)
						|| circleTL.intersects(shape)
						|| circleTR.intersects(shape)
						|| circleBR.intersects(shape)
						|| circleBL.intersects(shape));
			}
		};

		//
		//	http://www.phatcode.net/articles.php?id=459
		//
		[[nodiscard]]
		static bool CircleTriangleIntersection(const Circle& circle, const Triangle& triangle) noexcept
		{
			const double centerX = circle.x;
			const double centerY = circle.y;
			const double radius = circle.r;
			const double v1x = triangle.p0.x;
			const double v1y = triangle.p0.y;
			const double v2x = triangle.p1.x;
			const double v2y = triangle.p1.y;
			const double v3x = triangle.p2.x;
			const double v3y = triangle.p2.y;

			//
			// TEST 1: Vertex within circle
			//
			const double c1x = centerX - v1x;
			const double c1y = centerY - v1y;

			const double radiusSqr = radius * radius;
			const double c1sqr = c1x * c1x + c1y * c1y - radiusSqr;

			if (c1sqr <= 0)
			{
				return true;
			}

			const double c2x = centerX - v2x;
			const double c2y = centerY - v2y;
			const double c2sqr = c2x * c2x + c2y * c2y - radiusSqr;

			if (c2sqr <= 0)
			{
				return true;
			}

			const double c3x = centerX - v3x;
			const double c3y = centerY - v3y;

			//const double &c3sqr = radiusSqr;//; reference to radiusSqr
			const double c3sqr = c3x * c3x + c3y * c3y - radiusSqr;

			if (c3sqr <= 0)
			{
				return true;
			}

			//;
			//; TEST 2: Circle centre within triangle
			//;

			//;
			//; Calculate edges
			//;
			const double e1x = v2x - v1x;
			const double e1y = v2y - v1y;

			const double e2x = v3x - v2x;
			const double e2y = v3y - v2y;

			const double e3x = v1x - v3x;
			const double e3y = v1y - v3y;

			if (e1y * c1x >= e1x * c1y
				&& e2y * c2x >= e2x * c2y
				&& e3y * c3x >= e3x * c3y)
			{
				return true;
			}

			//;
			//; TEST 3: Circle intersects edge
			//;
			double k = c1x * e1x + c1y * e1y;

			if (k > 0)
			{
				const double len = e1x * e1x + e1y * e1y;//; squared len

				if (k < len)
				{
					if (c1sqr * len <= k * k)
					{
						return true;
					}
				}
			}

			//; Second edge
			k = c2x * e2x + c2y * e2y;

			if (k > 0)
			{
				const double len = e2x * e2x + e2y * e2y;

				if (k < len)
				{
					if (c2sqr * len <= k * k)
					{
						return true;
					}
				}
			}

			//; Third edge
			k = c3x * e3x + c3y * e3y;

			if (k > 0)
			{
				const double len = e3x * e3x + e3y * e3y;

				if (k < len)
				{
					if (c3sqr * len <= k * k)
					{
						return true;
					}
				}
			}

			// Within
			if (circle.center.intersects(triangle))
			{
				return true;
			}

			//; We're done, no intersection
			return false;
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Point, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const Point& a, const RoundRect& b) noexcept
		{
			return Intersect(Vec2{ a }, b);
		}

		bool Intersect(const Point& a, const Polygon& b) noexcept
		{
			return b.intersects(Vec2{ a });
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const Vec2& a, const RoundRect& b) noexcept
		{
			return RoundRectParts{ b }.intersects(a);
		}

		bool Intersect(const Vec2& a, const Polygon& b) noexcept
		{
			return b.intersects(a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Line, _)
		//
		////////////////////////////////////////////////////////////////

		//
		// `Intersect(const Line& a, const Line& b)` is based on
		// https://www.codeproject.com/Tips/862988/Find-the-Intersection-Point-of-Two-Line-Segments
		//
		// Licenced with the Code Project Open Licence (CPOL)
		// http://www.codeproject.com/info/cpol10.aspx
		//
		bool Intersect(const Line& a, const Line& b) noexcept
		{
			const Vec2 r = (a.end - a.start);
			const Vec2 s = (b.end - b.start);
			const Vec2 qp = (b.start - a.start);
			const double rxs = (r.x * s.y - r.y * s.x);
			const double qpxr = (qp.x * r.y - qp.y * r.x);
			const double qpxs = (qp.x * s.y - qp.y * s.x);

			if (IsZero(rxs))
			{
				if (IsZero(qpxr) && IsZero(qpxs))
				{
					const double qpr = qp.dot(r);
					const double q2pr = (b.end - a.start).dot(r);
					const double pqs = (a.start - b.start).dot(s);
					const double p2qs = (a.end - b.start).dot(s);

					const double rr = r.dot(r);
					const bool rrIsZero = IsZero(rr);
					const double ss = s.dot(s);
					const bool ssIsZero = IsZero(ss);

					if (rrIsZero && ssIsZero && IsZero(qp.dot(qp)))
					{
						// The two lines are both zero length and in the same position
						return true;
					}

					if (((not rrIsZero) && ((0 <= qpr && qpr <= rr) || (0 <= q2pr && q2pr <= rr)))
						|| ((not ssIsZero) && ((0 <= pqs && pqs <= ss) || (0 <= p2qs && p2qs <= ss))))
					{
						// Two lines are overlapping
						return true;
					}

					// Two lines are collinear but disjoint.
					return false;
				}

				// Two lines are parallel and non-intersecting.
				return false;
			}
			else
			{
				const double t = qpxs / rxs;
				const double u = qpxr / rxs;
				if ((0.0 <= t && t <= 1.0) && (0.0 <= u && u <= 1.0))
				{
					// An intersection was found
					return true;
				}

				// Two line segments are not parallel but do not intersect
				return false;
			}
		}

		bool Intersect(const Line& a, const Rect& b) noexcept
		{
			if (Intersect(a.start, b) || Intersect(a.end, b))
			{
				return true;
			}

			const Vec2 tl = b.tl(), tr = b.tr(), br = b.br(), bl = b.bl();

			return (Intersect(a, Line{ tl, tr })
				 || Intersect(a, Line{ tr, br })
				 || Intersect(a, Line{ br, bl })
				 || Intersect(a, Line{ bl, tl }));
		}

		bool Intersect(const Line& a, const RectF& b) noexcept
		{
			if (Intersect(a.start, b) || Intersect(a.end, b))
			{
				return true;
			}

			const Vec2 tl = b.tl(), tr = b.tr(), br = b.br(), bl = b.bl();

			return (Intersect(a, Line{ tl, tr })
				 || Intersect(a, Line{ tr, br })
				 || Intersect(a, Line{ br, bl })
				 || Intersect(a, Line{ bl, tl }));
		}

		bool Intersect(const Line& a, const Circle& b) noexcept
		{
			const Vec2 ab = (a.end - a.start);
			const Vec2 ac = (b.center - a.start);
			const double e = ac.dot(ab);
			const double rr = b.r * b.r;

			if (e <= 0.0)
			{
				return ac.dot(ac) <= rr;
			}

			const double f = ab.dot(ab);

			if (e >= f)
			{
				const Vec2 bc = b.center - a.end;
				return bc.dot(bc) <= rr;
			}

			return (ac.dot(ac) - e * e / f) <= rr;
		}

		//
		//	https://github.com/thelonious/kld-intersections/blob/development/lib/Intersection.js
		//
		bool Intersect(const Line& a, const Ellipse& b) noexcept
		{
			const double rx = b.a;
			const double ry = b.b;
			const Vec2 dir(a.vector());
			const Vec2 diff(a.start - b.center);
			const Vec2 mDir(dir.x / (rx * rx), dir.y / (ry * ry));
			const Vec2 mDiff(diff.x / (rx * rx), diff.y / (ry * ry));

			const double va = dir.dot(mDir);
			const double vb = dir.dot(mDiff);
			const double vc = diff.dot(mDiff) - 1.0;
			double vd = vb * vb - va * vc;

			const double ERRF = 1e-15;
			const double ZEROepsilon = 10 * Max({ std::abs(va), std::abs(vb), std::abs(vc) }) * ERRF;
			if (std::abs(vd) < ZEROepsilon)
			{
				vd = 0;
			}

			if (vd < 0)
			{
				return false;
			}
			else if (vd > 0)
			{
				const double root = std::sqrt(vd);
				double t_a = (-vb - root) / va;
				double t_b = (-vb + root) / va;
				t_b = (t_b > 1) ? t_b - ERRF : (t_b < 0) ? t_b + ERRF : t_b;
				t_a = (t_a > 1) ? t_a - ERRF : (t_a < 0) ? t_a + ERRF : t_a;

				if ((t_a < 0 || 1 < t_a) && (t_b < 0 || 1 < t_b))
				{
					return !((t_a < 0 && t_b < 0) || (t_a > 1 && t_b > 1));
				}
				else
				{
					return true;
				}
			}
			else
			{
				const double t = -vb / va;
				return (0 <= t && t <= 1);
			}
		}

		bool Intersect(const Line& a, const Triangle& b) noexcept
		{
			if (Intersect(a.start, b) || Intersect(a.end, b))
			{
				return true;
			}

			return (Intersect(a, Line{ b.p0, b.p1 })
				 || Intersect(a, Line{ b.p1, b.p2 })
				 || Intersect(a, Line{ b.p2, b.p0 }));
		}

		bool Intersect(const Line& a, const Quad& b) noexcept
		{
			if (Intersect(a.start, b) || Intersect(a.end, b))
			{
				return true;
			}

			return (Intersect(a, Line{ b.p0, b.p1 })
				 || Intersect(a, Line{ b.p1, b.p2 })
				 || Intersect(a, Line{ b.p2, b.p3 })
				 || Intersect(a, Line{ b.p3, b.p0 }));
		}

		bool Intersect(const Line& a, const RoundRect& b) noexcept
		{
			return RoundRectParts{ b }.intersects(a);
		}

		bool Intersect(const Line& a, const Polygon& b) noexcept
		{
			return b.intersects(a);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Rect, _)
		//
		//////////////////////////////////////////////////

		bool Intersect(const Rect& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Rect& a, const Triangle& b) noexcept
		{
			return (Intersect(a.triangleAtIndex(0), b)
				 || Intersect(a.triangleAtIndex(1), b));
		}

		bool Intersect(const Rect& a, const Quad& b) noexcept
		{
			return (Intersect(a, b.triangleAtIndex(0))
				 || Intersect(a, b.triangleAtIndex(1)));
		}

		bool Intersect(const Rect& a, const RoundRect& b) noexcept
		{
			return RoundRectParts{ b }.intersects(a);
		}

		bool Intersect(const Rect& a, const Polygon& b) noexcept
		{
			return b.intersects(a);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(RectF, _)
		//
		//////////////////////////////////////////////////

		bool Intersect(const RectF& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const RectF& a, const Triangle& b) noexcept
		{
			return (Intersect(a.triangleAtIndex(0), b)
				|| Intersect(a.triangleAtIndex(1), b));
		}

		bool Intersect(const RectF& a, const Quad& b) noexcept
		{
			return (Intersect(a, b.triangleAtIndex(0))
				|| Intersect(a, b.triangleAtIndex(1)));
		}

		bool Intersect(const RectF& a, const RoundRect& b) noexcept
		{
			return RoundRectParts{ b }.intersects(a);
		}

		bool Intersect(const RectF& a, const Polygon& b) noexcept
		{
			return b.intersects(a);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Circle, _)
		//
		//////////////////////////////////////////////////

		bool Intersect(const Circle& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Circle& a, const Triangle& b) noexcept
		{
			return CircleTriangleIntersection(a, b);
		}

		bool Intersect(const Circle& a, const Quad& b) noexcept
		{
			return (CircleTriangleIntersection(a, b.triangleAtIndex(0))
				 || CircleTriangleIntersection(a, b.triangleAtIndex(1)));
		}

		bool Intersect(const Circle& a, const RoundRect& b) noexcept
		{
			return RoundRectParts{ b }.intersects(a);
		}

		bool Intersect(const Circle& a, const Polygon& b) noexcept
		{
			return b.intersects(a);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Ellipse, _)
		//
		//////////////////////////////////////////////////

		bool Intersect(const Ellipse& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Ellipse& a, const Triangle& b) noexcept
		{
			Triangle triangle = b.movedBy(-a.center);

			const double v = (a.a / a.b);

			for (size_t i = 0; i < 3; ++i)
			{
				triangle.pointAtIndex(i).y *= v;
			}

			return Intersect(Circle{ a.a }, triangle);
		}

		bool Intersect(const Ellipse& a, const Quad& b) noexcept
		{
			Quad quad = b.movedBy(-a.center);

			const double v = (a.a / a.b);

			for (size_t i = 0; i < 4; ++i)
			{
				quad.pointAtIndex(i).y *= v;
			}

			return Intersect(Circle{ a.a }, quad);
		}

		bool Intersect(const Ellipse& a, const Polygon& b) noexcept
		{
			return b.intersects(a);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Triangle, _)
		//
		//////////////////////////////////////////////////

		bool Intersect(const Triangle& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Triangle& a, const Rect& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Triangle& a, const RectF& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Triangle& a, const Circle& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Triangle& a, const Ellipse& b) noexcept
		{
			return Intersect(b, a);
		}

		//
		//	http://marupeke296.com/COL_2D_TriTri.html
		//
		bool Intersect(const Triangle& a, const Triangle& b) noexcept
		{
			constexpr size_t other[3] = { 1, 2, 0 };
			constexpr size_t pindex[4] = { 1, 2, 0, 1 };
			const Triangle* tri[3] = { &a, &b, &a };

			for (int32 t = 0; t < 2; ++t)
			{
				const Triangle& ta = *tri[t];
				const Triangle& tb = *tri[t + 1];

				for (int32 i = 0; i < 3; ++i)
				{
					const Vec2 vec = (ta.pointAtIndex(pindex[i + 1]) - ta.pointAtIndex(pindex[i])).normalized();
					const Vec2 sepVec(vec.y, -vec.x);

					double s1min = sepVec.dot(ta.pointAtIndex(i));
					double s1max = sepVec.dot(ta.pointAtIndex(other[i]));

					if (s1max < s1min)
					{
						std::ranges::swap(s1min, s1max);
					}

					double s2min = sepVec.dot(tb.pointAtIndex(0));
					double s2max = sepVec.dot(tb.pointAtIndex(1));

					if (s2max < s2min)
					{
						std::ranges::swap(s2min, s2max);
					}

					const double d3 = sepVec.dot(tb.pointAtIndex(2));

					if (d3 < s2min)
					{
						s2min = d3;
					}
					else if (s2max < d3)
					{
						s2max = d3;
					}

					if (((s2min <= s1min) && (s1min <= s2max))
						|| ((s2min <= s1max) && (s1max <= s2max))
						|| ((s1min <= s2min) && (s2min <= s1max))
						|| ((s1min <= s2max) && (s2max <= s1max)))
					{
						continue;
					}

					return false;
				}
			}

			return true;
		}

		bool Intersect(const Triangle& a, const Quad& b) noexcept
		{
			return (Intersect(a, b.triangleAtIndex(0))
				 || Intersect(a, b.triangleAtIndex(1)));
		}

		bool Intersect(const Triangle& a, const RoundRect& b) noexcept
		{
			return RoundRectParts{ b }.intersects(a);
		}

		bool Intersect(const Triangle& a, const Polygon& b) noexcept
		{
			return b.intersects(a);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Quad, _)
		//
		//////////////////////////////////////////////////

		bool Intersect(const Quad& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Quad& a, const Rect& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Quad& a, const RectF& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Quad& a, const Circle& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Quad& a, const Ellipse& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Quad& a, const Triangle& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Quad& a, const Quad& b) noexcept
		{
			return (Intersect(a.triangleAtIndex(0), b)
				 || Intersect(a.triangleAtIndex(1), b));
		}

		bool Intersect(const Quad& a, const RoundRect& b) noexcept
		{
			return RoundRectParts{ b }.intersects(a);
		}

		bool Intersect(const Quad& a, const Polygon& b) noexcept
		{
			return b.intersects(a);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(RoundRect, _)
		//
		//////////////////////////////////////////////////

		bool Intersect(const RoundRect& a, const Point& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const RoundRect& a, const Vec2& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const RoundRect& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const RoundRect& a, const Rect& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const RoundRect& a, const RectF& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const RoundRect& a, const Circle& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const RoundRect& a, const Triangle& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const RoundRect& a, const Quad& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const RoundRect& a, const RoundRect& b) noexcept
		{
			if (not Intersect(a.rect, b.rect))
			{
				return false;
			}

			const RoundRectParts partsA{ a };
			const RoundRectParts partsB{ b };

			return (partsA.intersects(partsB.rectA)
				 || partsA.intersects(partsB.rectB)
				 || partsA.intersects(partsB.circleTL)
				 || partsA.intersects(partsB.circleTR)
				 || partsA.intersects(partsB.circleBR)
				 || partsA.intersects(partsB.circleBL));
		}

		bool Intersect(const RoundRect& a, const Polygon& b) noexcept
		{
			if (b.isEmpty())
			{
				return false;
			}

			if (not Intersect(a, b.boundingRect()))
			{
				return false;
			}

			const RoundRectParts partsA{ a };

			const size_t num_triangles = b.num_triangles();

			for (size_t i = 0; i < num_triangles; ++i)
			{
				if (partsA.intersects(b.triangleAtIndex(i)))
				{
					return true;
				}
			}

			return false;
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Polygon, _)
		//
		//////////////////////////////////////////////////

		bool Intersect(const Polygon& a, const Point& b) noexcept
		{
			return a.intersects(Vec2{ b });
		}

		bool Intersect(const Polygon& a, const Vec2& b) noexcept
		{
			return a.intersects(b);
		}

		bool Intersect(const Polygon& a, const Line& b) noexcept
		{
			return a.intersects(b);
		}

		bool Intersect(const Polygon& a, const Rect& b) noexcept
		{
			return a.intersects(b);
		}

		bool Intersect(const Polygon& a, const RectF& b) noexcept
		{
			return a.intersects(b);
		}

		bool Intersect(const Polygon& a, const Circle& b) noexcept
		{
			return a.intersects(b);
		}

		bool Intersect(const Polygon& a, const Ellipse& b) noexcept
		{
			return a.intersects(b);
		}

		bool Intersect(const Polygon& a, const Triangle& b) noexcept
		{
			return a.intersects(b);
		}

		bool Intersect(const Polygon& a, const Quad& b) noexcept
		{
			return a.intersects(b);
		}

		bool Intersect(const Polygon& a, const RoundRect& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Polygon& a, const Polygon& b) noexcept
		{
			return a.intersects(b);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(MultiPolygon, _)
		//
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//
		//	Intersect(LineString, _)
		//
		//////////////////////////////////////////////////

	}
}

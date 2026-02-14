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
//	Authors (OpenSiv3D challenge participants)
//	- Ebishu
//	- fal_rnd
//	- きつねび
//-----------------------------------------------

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Bezier.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/ListUtility.hpp>
# include <Siv3D/PolynomialSolver.hpp>
# include <Siv3D/Geometry2D/Intersect.hpp>
# include <Siv3D/Geometry2D/Distance.hpp>

namespace s3d
{
	namespace
	{
		constexpr double Epsilon = 1e-12;

		[[nodiscard]]
		static constexpr bool NearZero(const double v) noexcept
		{
			return (Abs(v) <= Epsilon);
		}

		[[nodiscard]]
		static constexpr bool In01(const double t) noexcept
		{
			return ((-Epsilon <= t) && (t <= (1.0 + Epsilon)));
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

			if (triangle.hasArea()
				&& e1y * c1x >= e1x * c1y
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

		bool Intersect(const Point& a, const Bezier2& b)
		{
			return Intersect(Vec2{ a }, b);
		}

		bool Intersect(const Point& a, const Bezier3& b)
		{
			return Intersect(Vec2{ a }, b);
		}

		bool Intersect(const Point& a, const RoundRect& b) noexcept
		{
			return Intersect(Vec2{ a }, b);
		}

		bool Intersect(const Point& a, const SuperEllipse& b) noexcept
		{
			return Intersect(Vec2{ a }, b);
		}

		bool Intersect(const Point& a, const Polygon& b) noexcept
		{
			return b.intersects(Vec2{ a });
		}

		bool Intersect(const Point& a, const MultiPolygon& b) noexcept
		{
			return Intersect(Vec2{ a }, b);
		}

		bool Intersect(const Point& a, const LineString& b) noexcept
		{
			return Intersect(Vec2{ a }, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const Vec2& a, const Bezier2& b)
		{
			// 距離が 1 未満なら交差と判定
			return (Distance(a, b) < 1.0);
		}

		bool Intersect(const Vec2& a, const Bezier3& b)
		{
			// 距離が 1 未満なら交差と判定
			return (Distance(a, b) < 1.0);
		}

		bool Intersect(const Vec2& a, const RoundRect& b) noexcept
		{
			return RoundRectParts{ b }.intersects(a);
		}

		bool Intersect(const Vec2& a, const SuperEllipse& b) noexcept
		{
			const double ax = Abs(b.axes.x);
			const double by = Abs(b.axes.y);

			if ((ax == 0.0) || (by == 0.0))
			{
				return false;
			}

			if (b.n <= 0.0)
			{
				return false;
			}

			const double dx = Abs((a.x - b.center.x) / ax);
			const double dy = Abs((a.y - b.center.y) / by);

			if ((1.0 < dx) && (1.0 < dy))
			{
				return false;
			}

			const double v = (std::pow(dx, b.n) + std::pow(dy, b.n));
			return (v <= 1.0);
		}

		bool Intersect(const Vec2& a, const Polygon& b) noexcept
		{
			return b.intersects(a);
		}

		bool Intersect(const Vec2& a, const MultiPolygon& b) noexcept
		{
			for (const auto& polygon : b)
			{
				if (Intersect(a, polygon))
				{
					return true;
				}
			}

			return false;
		}

		bool Intersect(const Vec2& a, const LineString& b) noexcept
		{
			const size_t n = b.size();
			
			if (n < 2)
			{
				return false;
			}

			constexpr double Pad = 1.0;
			const RectF boundingRect = b.computeBoundingRect().stretched(Pad);

			if (not boundingRect.contains(a))
			{
				return false;
			}

			const Vec2* pPoint = b.data();
			Vec2 p0 = pPoint[0];

			for (size_t i = 0; i < (n - 1); ++i)
			{
				const Vec2 p1 = *(++pPoint);

				auto [minX, maxX] = MinMax(p0.x, p1.x);
				minX -= Pad; maxX += Pad;

				auto [minY, maxY] = MinMax(p0.y, p1.y);
				minY -= Pad; maxY += Pad;

				if ((a.x < minX) || (maxX < a.x) || (a.y < minY) || (maxY < a.y))
				{
					p0 = p1;
					continue;
				}

				if (detail::DistanceSq(p0, p1, a) < 1.0)
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Line, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const Line& a, const Line& b) noexcept
		{
			const Vec2 p = a.start;
			const Vec2 r = (a.end - a.start);
			const Vec2 q = b.start;
			const Vec2 s = (b.end - b.start);

			const Vec2 qp = (q - p);

			const double rxs = (r.x * s.y - r.y * s.x);
			const double qpxr = (qp.x * r.y - qp.y * r.x);

			const double rr = r.dot(r);
			const double ss = s.dot(s);

			const bool rIsPoint = NearZero(rr);
			const bool sIsPoint = NearZero(ss);

			// a, b 両方が点
			if (rIsPoint && sIsPoint)
			{
				return NearZero((a.start - b.start).dot(a.start - b.start));
			}

			// a が点（点が b 上にあるか）
			if (rIsPoint)
			{
				// b も点は上で処理済みなのでここでは sIsPoint=false
				const Vec2 ap = (a.start - b.start);
				const double cross = (ap.x * s.y - ap.y * s.x);
				
				if (not NearZero(cross))
				{
					return false;
				}

				const double t = (ap.dot(s) / ss);
				return In01(t);
			}

			// b が点（点が a 上にあるか）
			if (sIsPoint)
			{
				const Vec2 bp = (b.start - a.start);
				const double cross = (bp.x * r.y - bp.y * r.x);
				
				if (not NearZero(cross))
				{
					return false;
				}

				const double t = (bp.dot(r) / rr);
				return In01(t);
			}

			// 平行
			if (NearZero(rxs))
			{
				if (not NearZero(qpxr))
				{
					return false; // 平行で別線
				}

				// 同一直線：a 上の 1D 区間重なり判定
				const double t0 = ((b.start - a.start).dot(r)) / rr;
				const double t1 = ((b.end - a.start).dot(r)) / rr;

				const auto [lo, hi] = MinMax(t0, t1);

				// [lo,hi] と [0,1] が重なるか
				return ((-Epsilon <= hi) && (lo <= (1.0 + Epsilon)));
			}

			// 一般位置
			const double qpxs = (qp.x * s.y - qp.y * s.x);
			const double t = (qpxs / rxs);
			const double u = (qpxr / rxs);

			return (In01(t) && In01(u));
		}

		bool Intersect(const Line& a, const Bezier2& b)
		{
			const double a1 = (a.start.y - a.end.y);
			const double b1 = (a.end.x - a.start.x);
			const double c1 = a.start.cross(a.end);

			const double a2 = (a1 * (b.p0.x - 2 * b.p1.x + b.p2.x) + b1 * (b.p0.y - 2 * b.p1.y + b.p2.y));
			const double b2 = (2 * (a1 * (b.p1 - b.p0).x + b1 * (b.p1 - b.p0).y));
			const double c2 = std::fma(a1, b.p0.x, std::fma(b1, b.p0.y, c1));

			if (const auto opt = Math::SolveQuadraticEquation(a2, b2, c2))
			{
				for (const double x : *opt)
				{
					if (In01(x))
					{
						const Vec2 intersection = b.getPos(Clamp(x, 0.0, 1.0));

						if (Intersect(a, intersection))
						{
							return true;
						}
					}
				}
			}

			return false;
		}

		bool Intersect(const Line& a, const Bezier3& b)
		{
			const Vec2 p0 = b.p0, p1 = b.p1, p2 = b.p2, p3 = b.p3;
			const double a1 = (a.start.y - a.end.y);
			const double b1 = (a.end.x - a.start.x);
			const double c1 = a.start.cross(a.end);

			std::array<Vec2, 4> v = { (-p0 + 3 * p1 - 3 * p2 + p3), (3 * p0 - 6 * p1 + 3 * p2), (-3 * p0 + 3 * p1), p0 };
			std::array<double, 4> t;
			for (size_t i = 0; i < 4; ++i)
			{
				t[i] = (a1 * v[i].x + b1 * v[i].y);
			}
			t[3] += c1;

			if (const auto opt = Math::SolveCubicEquation(t[0], t[1], t[2], t[3]))
			{
				for (const double x : *opt)
				{
					if (In01(x))
					{
						const Vec2 intersection = b.getPos(Clamp(x, 0.0, 1.0));

						if (Intersect(a, intersection))
						{
							return true;
						}
					}
				}
			}

			return false;
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
			const double rr = (b.r * b.r);

			if (e <= 0.0)
			{
				return (ac.dot(ac) <= rr);
			}

			const double f = ab.dot(ab);

			if (f <= e)
			{
				const Vec2 bc = (b.center - a.end);
				return (bc.dot(bc) <= rr);
			}

			return ((ac.dot(ac) - e * e / f) <= rr);
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

		bool Intersect(const Line& a, const SuperEllipse& b) noexcept
		{
			const double ax = Abs(b.axes.x);
			const double by = Abs(b.axes.y);

			if ((ax == 0.0) || (by == 0.0) || (b.n <= 0.0))
			{
				return false;
			}

			// 線分が点なら点判定
			if (a.start == a.end)
			{
				return Intersect(a.start, b);
			}

			// AABB
			if (not Intersect(a, b.boundingRect()))
			{
				return false;
			}

			// bounding circle
			if (not Intersect(a, b.boundingCircle()))
			{
				return false;
			}

			// 線分の両端が内部にあるか
			if (Intersect(a.start, b) || Intersect(a.end, b))
			{
				return true;
			}

			const Vec2 p0 = ((a.start - b.center) / Vec2{ ax, by });
			const Vec2 p1 = ((a.end - b.center) / Vec2{ ax, by });
			const Vec2 dir = (p1 - p0);

			const auto Eval = [&](double t) noexcept -> double
			{
				const double x = (p0.x + dir.x * t);
				const double y = (p0.y + dir.y * t);
				return (std::pow(Abs(x), b.n) + std::pow(Abs(y), b.n));
			};

			double lo = 0.0;
			double hi = 1.0;
			constexpr double phi = 0.6180339887498948482;

			double c = (hi - (hi - lo) * phi);
			double d = (lo + (hi - lo) * phi);

			double fc = Eval(c);
			double fd = Eval(d);

			for (int32 i = 0; i < 24; ++i)
			{
				if ((fc <= 1.0) || (fd <= 1.0))
				{
					return true;
				}

				if (fc < fd)
				{
					hi = d;
					d = c;
					fd = fc;
					c = (hi - (hi - lo) * phi);
					fc = Eval(c);
				}
				else
				{
					lo = c;
					c = d;
					fc = fd;
					d = (lo + (hi - lo) * phi);
					fd = Eval(d);
				}
			}

			const double tMin = (0.5 * (lo + hi));
			return (Eval(tMin) <= 1.0);
		}

		bool Intersect(const Line& a, const Polygon& b) noexcept
		{
			return b.intersects(a);
		}

		bool Intersect(const Line& a, const MultiPolygon& b) noexcept
		{
			for (const auto& polygon : b)
			{
				if (Intersect(a, polygon))
				{
					return true;
				}
			}

			return false;
		}

		bool Intersect(const Line& a, const LineString& b) noexcept
		{
			const size_t n = b.size();

			if (n < 2)
			{
				return false;
			}

			// AABB による判定
			constexpr double Pad = 1.0;
			const RectF br = b.computeBoundingRect().stretched(Pad);
			const RectF ar = RectF::FromPoints(a.start, a.end).stretched(Pad);
			const double aLeft		= ar.leftX();
			const double aRight		= ar.rightX();
			const double aTop		= ar.topY();
			const double aBottom	= ar.bottomY();

			if (not br.intersects(ar))
			{
				return false;
			}

			const Vec2* pPoint = b.data();
			Vec2 p0 = pPoint[0];

			for (size_t i = 0; i < (n - 1); ++i)
			{
				const Vec2 p1 = *(++pPoint);

				// 各セグメントの AABB で足切り
				auto [minX, maxX] = MinMax(p0.x, p1.x);
				auto [minY, maxY] = MinMax(p0.y, p1.y);
				minX -= Pad; maxX += Pad;
				minY -= Pad; maxY += Pad;

				// Line a の AABB と重ならないなら交差しない
				if ((aRight < minX) || (maxX < aLeft) || (aBottom < minY) || (maxY < aTop))
				{
					p0 = p1;
					continue;
				}

				// 本判定
				if (Intersect(a, Line{ p0, p1 }))
				{
					return true;
				}

				p0 = p1;
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Bezier2, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const Bezier2& a, const Point& b)
		{
			return Intersect(b, a);
		}

		bool Intersect(const Bezier2& a, const Vec2& b)
		{
			return Intersect(b, a);
		}

		bool Intersect(const Bezier2& a, const Line& b)
		{
			return Intersect(b, a);
		}

		bool Intersect(const Bezier2& a, const Bezier2& b)
		{
			const RectF aRect = a.boundingRect();
			const RectF bRect = b.boundingRect();

			if (not Intersect(aRect, bRect))
			{
				return false;
			}

			return Intersect(a.getLineStringAdaptive(), b.getLineStringAdaptive());
		}

		bool Intersect(const Bezier2& a, const Bezier3& b)
		{
			const RectF aRect = a.boundingRect();
			const RectF bRect = b.boundingRect();

			if (not Intersect(aRect, bRect))
			{
				return false;
			}

			return Intersect(a.getLineStringAdaptive(), b.getLineStringAdaptive());
		}

		bool Intersect(const Bezier2& a, const Rect& b)
		{
			return Intersect(a, RectF{ b });
		}

		bool Intersect(const Bezier2& a, const RectF& b)
		{
			// 端点が内側にある場合
			if (Intersect(a.p0, b) || Intersect(a.p2, b))
			{
				return true;
			}

			// 制御点の AABB と矩形が交差しない場合は交差しない
			const RectF bezierAABB = a.controlPointsBoundingRect();
			
			if (not Intersect(bezierAABB, b))
			{
				return false;
			}

			// 長方形の各辺と交差するか
			for (const Line& line : { b.top(), b.right(), b.bottom(), b.left() })
			{
				// 線分の AABB と制御点の AABB が交差しない場合は交差しない
				if (not Intersect(bezierAABB, line))
				{
					continue;
				}

				// 線分とベジェ曲線の交差判定（計算コスト高）
				if (Intersect(line, a))
				{
					return true;
				}
			}

			return false;
		}

		bool Intersect(const Bezier2& a, const Circle& b)
		{
			// 制御点の AABB が交差しない場合は交差しない
			{
				const RectF bezierAABB = a.controlPointsBoundingRect();

				if (not Intersect(bezierAABB, b))
				{
					return false;
				}
			}

			// 端点が円の内側にあれば交差
			if (Intersect(a.p0, b) || Intersect(a.p2, b))
			{
				return true;
			}

			const Vec2 closestPoint = a.closestPoint(b.center);
			return (closestPoint.distanceFromSq(b.center) <= (b.r * b.r));
		}

		bool Intersect(const Bezier2& a, const Ellipse& b)
		{
			// 制御点の AABB が交差しない場合は交差しない
			{
				const RectF bezierAABB = a.controlPointsBoundingRect();

				if (not Intersect(bezierAABB, b))
				{
					return false;
				}
			}

			// 端点のいずれかが楕円の内側にあれば交差
			if (Intersect(a.p0, b) || Intersect(a.p2, b))
			{
				return true;
			}
			
			const Vec2 invR{ (1.0 / b.a), (1.0 / b.b) };
			Bezier2 localBezier;
			localBezier.p0 = ((a.p0 - b.center) * invR);
			localBezier.p1 = ((a.p1 - b.center) * invR);
			localBezier.p2 = ((a.p2 - b.center) * invR);

			const double distanceSq = localBezier.closestPoint(Vec2{ 0, 0 }).lengthSq();
			return (distanceSq <= 1.0);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Bezier3, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const Bezier3& a, const Point& b)
		{
			return Intersect(b, a);
		}

		bool Intersect(const Bezier3& a, const Vec2& b)
		{
			return Intersect(b, a);
		}

		bool Intersect(const Bezier3& a, const Line& b)
		{
			return Intersect(b, a);
		}

		bool Intersect(const Bezier3& a, const Bezier2& b)
		{
			return Intersect(b, a);
		}

		bool Intersect(const Bezier3& a, const Bezier3& b)
		{
			const RectF aRect = a.boundingRect();
			const RectF bRect = b.boundingRect();

			if (not Intersect(aRect, bRect))
			{
				return false;
			}

			return Intersect(a.getLineStringAdaptive(), b.getLineStringAdaptive());
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Rect, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const Rect& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Rect& a, const Bezier2& b)
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

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(RectF, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const RectF& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const RectF& a, const Bezier2& b)
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

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Circle, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const Circle& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Circle& a, const Bezier2& b)
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

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Ellipse, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const Ellipse& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}

		bool Intersect(const Ellipse& a, const Bezier2& b)
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

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Triangle, _)
		//
		////////////////////////////////////////////////////////////////

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

		bool Intersect(const Triangle& a, const Triangle& b) noexcept
		{
			auto Project = [](const Triangle& t, const Vec2& axis) noexcept -> std::pair<double, double>
			{
				double mn = axis.dot(t.pointAtIndex(0));
				double mx = mn;

				for (int32 i = 1; i < 3; ++i)
				{
					const double d = axis.dot(t.pointAtIndex(i));
					
					if (d < mn)
					{
						mn = d;
					}
					
					if (mx < d)
					{
						mx = d;
					}
				}

				return{ mn, mx };
			};

			auto Overlap = [](double minA, double maxA, double minB, double maxB) noexcept
			{
				return not((maxA < minB) || (maxB < minA));
			};

			auto TestAxesFrom = [&](const Triangle& t0, const Triangle& t1) noexcept -> bool
			{
				for (int32 i = 0; i < 3; ++i)
				{
					const Vec2 p0 = t0.pointAtIndex(i);
					const Vec2 p1 = t0.pointAtIndex((i + 1) % 3);

					const Vec2 edge = (p1 - p0);

					// 軸 = edge の垂直
					const Vec2 axis(edge.y, -edge.x);

					// 退化（三角形が潰れている/辺がゼロ）
					if ((axis.x == 0.0) && (axis.y == 0.0))
					{
						continue;
					}

					const auto [min0, max0] = Project(t0, axis);
					const auto [min1, max1] = Project(t1, axis);

					if (not Overlap(min0, max0, min1, max1))
					{
						return false; // 分離軸あり
					}
				}
				return true;
			};

			// a の3軸 + b の3軸（合計6軸）
			return (TestAxesFrom(a, b) && TestAxesFrom(b, a));
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

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Quad, _)
		//
		////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(RoundRect, _)
		//
		////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Polygon, _)
		//
		////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(MultiPolygon, _)
		//
		////////////////////////////////////////////////////////////////


		////////////////////////////////////////////////////////////////
		//
		//	Intersect(LineString, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const LineString& a, const LineString& b) noexcept
		{
			const size_t an = a.size();
			const size_t bn = b.size();

			if ((an < 2) || (bn < 2))
			{
				return false;
			}

			constexpr double Pad = 1.0;

			// 全体 AABB による足切り
			const RectF ar = a.computeBoundingRect().stretched(Pad);
			const RectF br = b.computeBoundingRect().stretched(Pad);

			if (not ar.intersects(br))
			{
				return false;
			}

			// 外側ループを短い方にすると少し得
			const LineString* pOuter = &a;
			const LineString* pInner = &b;
			size_t on = an;
			size_t in = bn;

			if (bn < an)
			{
				pOuter = &b;
				pInner = &a;
				on = bn;
				in = an;
			}

			const Vec2* oPtr = pOuter->data();
			Vec2 o0 = oPtr[0];

			for (size_t oi = 0; oi < (on - 1); ++oi)
			{
				const Vec2 o1 = *(++oPtr);

				// Outer セグメント AABB
				auto [oMinX, oMaxX] = MinMax(o0.x, o1.x);
				auto [oMinY, oMaxY] = MinMax(o0.y, o1.y);
				oMinX -= Pad; oMaxX += Pad;
				oMinY -= Pad; oMaxY += Pad;

				// Inner 全体 AABB と重ならないなら、この Outer セグメントは無視
				{
					const double iLeft = br.leftX();
					const double iRight = br.rightX();
					const double iTop = br.topY();
					const double iBottom = br.bottomY();

					if ((oMaxX < iLeft) || (iRight < oMinX) || (oMaxY < iTop) || (iBottom < oMinY))
					{
						o0 = o1;
						continue;
					}
				}

				const Line outerSeg{ o0, o1 };

				// Inner の全セグメントと突き合わせ
				const Vec2* iPtr = pInner->data();
				Vec2 i0 = iPtr[0];

				for (size_t ii = 0; ii < (in - 1); ++ii)
				{
					const Vec2 i1 = *(++iPtr);

					// Inner セグメント AABB
					auto [iMinX, iMaxX] = MinMax(i0.x, i1.x);
					auto [iMinY, iMaxY] = MinMax(i0.y, i1.y);
					iMinX -= Pad; iMaxX += Pad;
					iMinY -= Pad; iMaxY += Pad;

					// AABB が重ならないなら交差しない
					if ((oMaxX < iMinX) || (iMaxX < oMinX) || (oMaxY < iMinY) || (iMaxY < oMinY))
					{
						i0 = i1;
						continue;
					}

					// 本判定（Line vs Line）
					if (Intersect(outerSeg, Line{ i0, i1 }))
					{
						return true;
					}

					i0 = i1;
				}

				o0 = o1;
			}

			return false;
		}
	}
}

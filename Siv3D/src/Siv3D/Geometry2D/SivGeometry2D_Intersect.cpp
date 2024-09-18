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
//	Authors (OpenSiv3D challenge participants)
//	- Ebishu
//	- fal_rnd
//	- きつねび
//-----------------------------------------------

# include <Siv3D/2DShapes.hpp>
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

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersect(const Vec2& a, const RoundRect& b) noexcept
		{
			return RoundRectParts{ b }.intersects(a);
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

		//////////////////////////////////////////////////
		//
		//	Intersect(Rect, _)
		//
		//////////////////////////////////////////////////

		bool Intersect(const Rect& a, const Line& b) noexcept
		{
			return Intersect(b, a);
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

		//////////////////////////////////////////////////
		//
		//	Intersect(Circle, _)
		//
		//////////////////////////////////////////////////

		bool Intersect(const Circle& a, const Line& b) noexcept
		{
			return Intersect(b, a);
		}
	}
}

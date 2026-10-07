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

# include <Siv3D/Bezier.hpp>
# include <Siv3D/LineCap.hpp>
# include <Siv3D/FloatFormatter.hpp>
# include <Siv3D/Geometry2D/BezierGeometry.hpp>
# include "Bezier3ArcLength.hpp"
# include "Bezier3Flatten.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	tangentAt
	//
	////////////////////////////////////////////////////////////////

	Vec2 Bezier3::tangentAt(const double t) const noexcept
	{
		return derivativeAt(t).normalized();
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalAt
	//
	////////////////////////////////////////////////////////////////

	Vec2 Bezier3::normalAt(const double t) const noexcept
	{
		const Vec2 d = derivativeAt(t);
		return Vec2{ d.y, -d.x }.normalized();
	}

	////////////////////////////////////////////////////////////////
	//
	//	headingAt
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::headingAt(const double t) const noexcept
	{
		return derivativeAt(t).getAngle();
	}

	////////////////////////////////////////////////////////////////
	//
	//	curvatureAt
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::curvatureAt(const double t) const noexcept
	{
		const Vec2 d1 = derivativeAt(t);          // B'(t)
		const Vec2 d2 = secondDerivativeAt(t);    // B''(t)

		const double speed2 = d1.lengthSq();

		if (speed2 == 0.0)
		{
			return 0.0;
		}

		const double cross = d1.cross(d2);
		const double denom = (speed2 * std::sqrt(speed2));
		return (cross / denom);
	}

	////////////////////////////////////////////////////////////////
	//
	//	radiusOfCurvatureAt
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::radiusOfCurvatureAt(const double t) const noexcept
	{
		const double curvature = curvatureAt(t);

		if (curvature == 0.0)
		{
			return Math::Inf;
		}

		return (1.0 / Abs(curvature));
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeLength
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::computeLength() const noexcept
	{
		return detail::BuildBezier3ArcLengthTable(*this).length();
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeTAtDistance
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::computeTAtDistance(const double distanceFromStart) const noexcept
	{
		if (distanceFromStart <= 0.0)
		{
			return 0.0;
		}
		return detail::Bezier3TAtDistance(*this, detail::BuildBezier3ArcLengthTable(*this), distanceFromStart);
	}

	////////////////////////////////////////////////////////////////
	//
	//	computePointAtDistance
	//
	////////////////////////////////////////////////////////////////

	Bezier3::position_type Bezier3::computePointAtDistance(const double distanceFromStart) const noexcept
	{
		return pointAt(computeTAtDistance(distanceFromStart));
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeClosestT
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::computeClosestT(const position_type& targetPoint) const noexcept
	{
		return detail::ClosestPointOnBezier(*this, targetPoint).parameter;
	}

	////////////////////////////////////////////////////////////////
	//
	//  computeClosestPoint
	//
	////////////////////////////////////////////////////////////////

	Bezier3::position_type Bezier3::computeClosestPoint(const position_type& targetPoint) const noexcept
	{
		return detail::ClosestPointOnBezier(*this, targetPoint).point;
	}

	////////////////////////////////////////////////////////////////
	//
	//	toLineString
	//
	////////////////////////////////////////////////////////////////

	LineString Bezier3::toLineString(int32 segments) const
	{
		segments = Max(1, segments);

		LineString pts(segments + 1);
		Vec2* pDst = pts.data();
		{
			// 始点
			*pDst++ = p0;

			for (int32 i = 1; i < segments; ++i)
			{
				const double t = (static_cast<double>(i) / segments);
				*pDst++ = pointAt(t);
			}

			// 終点
			*pDst = p3;
		}

		return pts;
	}

	////////////////////////////////////////////////////////////////
	//
	//	toLineStringAdaptive
	//
	////////////////////////////////////////////////////////////////

	LineString Bezier3::toLineStringAdaptive(const double maxError, const int32 maxDepth) const
	{
		LineString result;
		result.push_back(p0);
		detail::AppendBezier3Polyline(result, *this, (maxError * maxError), Max(0, maxDepth));
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	subcurve
	//
	////////////////////////////////////////////////////////////////

	Bezier3 Bezier3::subcurve(double t0, double t1) const noexcept
	{
		t0 = Clamp(t0, 0.0, 1.0);
		t1 = Clamp(t1, 0.0, 1.0);

		if ((t0 == 0.0) && (t1 == 1.0))
		{
			return *this;
		}

		if (t0 == t1)
		{
			const Vec2 p = pointAt(t0);
			return{ p, p, p, p };
		}

		if (t1 < t0)
		{
			return subcurve(t1, t0).reversed();
		}

		// まず t1 で split して左側 [0, t1] を取る
		const auto [left, _unused1] = split(t1);

		// 次に left を (t0/t1) で split して右側 [t0, t1] を取る
		return left.split(t0 / t1).second;
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeBoundingRect
	//
	////////////////////////////////////////////////////////////////

	RectF Bezier3::computeBoundingRect() const noexcept
	{
		constexpr double Eps = (64.0 * std::numeric_limits<double>::epsilon());
		constexpr double tTol = (16.0 * Eps);

		auto Update1D = [](double p0, double p1, double p2, double p3, double& mn, double& mx) noexcept
		{
			// 多項式 P(t) = A t^3 + B t^2 + C t + D
			const double A = (-p0 + 3.0 * p1 - 3.0 * p2 + p3);
			const double B = (3.0 * p0 - 6.0 * p1 + 3.0 * p2);
			const double C = (-3.0 * p0 + 3.0 * p1);
			const double D = p0;

			// まず端点で初期化
			mn = Min(p0, p3);
			mx = Max(p0, p3);

			// 評価関数: P(t) の計算
			auto Eval = [&](double t) noexcept
			{
				return std::fma(std::fma(std::fma(A, t, B), t, C), t, D);
			};

			// 候補 t の採用判定
			auto ConsiderT = [&](double t) noexcept
			{
				if (InRange(t, -tTol, (1.0 + tTol)))
				{
					t = Clamp(t, 0.0, 1.0);
					const double v = Eval(t);
					mn = Min(mn, v);
					mx = Max(mx, v);
				}
			};

			// 係数のスケール
			const double scale = Max({ Abs(p0), Abs(p1), Abs(p2), Abs(p3), 1.0 });

			// 導関数 P'(t) = 3A t^2 + 2B t + C = 0 を解く
			const double qa = (3.0 * A);
			const double qb = (2.0 * B);
			const double qc = C;

			// qa が極小なら、P'(t) は一次以下
			if (Abs(qa) <= (Eps * scale))
			{
				// qb も極小なら P'(t) は定数 → 極値なし
				if (Abs(qb) <= (Eps * scale))
				{
					return;
				}

				// 一次方程式 2B t + C = 0 を解く
				ConsiderT(-qc / qb);
				return;
			}

			// 二次方程式 qa t^2 + qb t + qc = 0 を解く
			const double disc = std::fma(qb, qb, -4.0 * qa * qc);
			if (disc < 0.0)
			{
				return; // 実根なし
			}

			const double s = std::sqrt(disc);
			// 桁落ちを防ぐための符号選択
			const double q = (-0.5) * (qb + (qb >= 0.0 ? s : -s));

			if (q == 0.0) // qb=0 かつ distanceFromStart=0 のケース等
			{
				ConsiderT(-qb / (2.0 * qa));
				return;
			}

			const double t0 = (q / qa);
			const double t1 = (qc / q);

			ConsiderT(t0);
			ConsiderT(t1);
		};

		double minX, maxX, minY, maxY;
		Update1D(p0.x, p1.x, p2.x, p3.x, minX, maxX);
		Update1D(p0.y, p1.y, p2.y, p3.y, minY, maxY);
		return{ minX, minY, (maxX - minX), (maxY - minY) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeInflectionTs
	//
	////////////////////////////////////////////////////////////////

	Array<double> Bezier3::computeInflectionTs() const
	{
		constexpr double Eps = 1e-12;

		Array<double> ts;

		// B'(t) = 3(A t^2 + B t + C)
		// B''(t)= 6(A t + B)
		// where
		// A = -p0 + 3p1 - 3p2 + p3
		// B =  p0 - 2p1 + p2
		// C =  p1 - p0
		//
		// Inflection (2D): cross(B'(t), B''(t)) = 0
		// => cross(A,B) t^2 + cross(A,C) t + cross(B,C) = 0
		const Vec2 A = (-p0 + (p1 * 3.0) - (p2 * 3.0) + p3);
		const Vec2 B = (p0 - (p1 * 2.0) + p2);
		const Vec2 C = (p1 - p0);

		const double c2 = A.cross(B);
		const double c1 = A.cross(C);
		const double c0 = B.cross(C);

		// Degenerate -> no reliable inflection candidates
		const double scale = Max({ Abs(c2), Abs(c1), Abs(c0), 1.0 });
		if (Abs(c2) <= (Eps * scale) && Abs(c1) <= (Eps * scale) && Abs(c0) <= (Eps * scale))
		{
			return ts;
		}

		auto PushIf01 = [&](double t) noexcept
		{
			if (InRange(t, 0.0, 1.0))
			{
				// avoid near-duplicates
				for (const double u : ts)
				{
					if (Abs(u - t) <= 1e-10)
					{
						return;
					}
				}
				ts.push_back(t);
			}
		};

		// If quadratic term is tiny -> linear
		if (Abs(c2) <= (Eps * scale))
		{
			if (Abs(c1) <= (Eps * scale))
			{
				return ts; // constant != 0 => no roots; constant ~ 0 handled above
			}

			const double t = (-c0 / c1);
			PushIf01(t);
			return ts;
		}

		// Quadratic roots
		const double disc = (c1 * c1 - 4.0 * c2 * c0);
		if (disc < 0.0)
		{
			return ts;
		}

		const double s = std::sqrt(disc);

		// Stable quadratic formula
		const double q = (-0.5) * (c1 + (c1 >= 0.0 ? s : -s));

		if (q == 0.0)
		{
			const double t = (-c1 / (2.0 * c2));
			PushIf01(t);
			return ts;
		}

		const double t0 = (q / c2);
		const double t1 = (c0 / q);

		PushIf01(t0);
		PushIf01(t1);

		if (ts.size() == 2)
		{
			if (ts[1] < ts[0])
			{
				std::swap(ts[0], ts[1]);
			}
		}

		return ts;
	}

	////////////////////////////////////////////////////////////////
	//
	//	paint
	//
	////////////////////////////////////////////////////////////////

	const Bezier3& Bezier3::paint(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paint(dst, LineCap::Round, 1.0, color, enableAntialiasing);
	}

	const Bezier3& Bezier3::paint(Image& dst, double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return paint(dst, LineCap::Round, thickness, color, enableAntialiasing);
	}

	const Bezier3& Bezier3::paint(Image& dst, const LineCap lineCap, double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		toLineStringAdaptive().paint(dst, lineCap, thickness, color, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	overwrite
	//
	////////////////////////////////////////////////////////////////

	const Bezier3& Bezier3::overwrite(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwrite(dst, LineCap::Round, 1.0, color, enableAntialiasing);
	}

	const Bezier3& Bezier3::overwrite(Image& dst, double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		return overwrite(dst, LineCap::Round, thickness, color, enableAntialiasing);
	}

	const Bezier3& Bezier3::overwrite(Image& dst, const LineCap lineCap, double thickness, const Color& color, const EnableAntialiasing enableAntialiasing) const
	{
		toLineStringAdaptive().overwrite(dst, lineCap, thickness, color, enableAntialiasing);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Bezier3& Bezier3::draw(const ColorF& color, const int32 segments) const
	{
		toLineString(segments).draw(color);
		return *this;
	}

	const Bezier3& Bezier3::draw(const double thickness, const ColorF& color, const int32 segments) const
	{
		toLineString(segments).draw(thickness, color);
		return *this;
	}

	const Bezier3& Bezier3::draw(const LineCap linaCap, const double thickness, const ColorF& color, const int32 segments) const
	{
		toLineString(segments).draw(linaCap, thickness, color);
		return *this;
	}

	const Bezier3& Bezier3::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& color, const int32 segments) const
	{
		toLineString(segments).draw(startCap, endCap, thickness, color);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawAdaptive
	//
	////////////////////////////////////////////////////////////////

	const Bezier3& Bezier3::drawAdaptive(const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		toLineStringAdaptive(maxError, maxDepth).draw(color);
		return *this;
	}

	const Bezier3& Bezier3::drawAdaptive(const double thickness, const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		toLineStringAdaptive(maxError, maxDepth).draw(thickness, color);
		return *this;
	}

	const Bezier3& Bezier3::drawAdaptive(const LineCap linaCap, const double thickness, const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		toLineStringAdaptive(maxError, maxDepth).draw(linaCap, thickness, color);
		return *this;
	}

	const Bezier3& Bezier3::drawAdaptive(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		toLineStringAdaptive(maxError, maxDepth).draw(startCap, endCap, thickness, color);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Bezier3& value)
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

	void Bezier3::ThrowControlPointAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Bezier3::controlPointAtIndex() index out of range" };
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Bezier3>::format(const s3d::Bezier3& value, fmt::format_context& ctx) const
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "(({}, {}), ({}, {}), ({}, {}), ({}, {}))", value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y, value.p3.x, value.p3.y);
	}
	else
	{
		const std::string format
			= ("(({:" + tag + "}, {:" + tag + "}), ({:" + tag + "}, {:" + tag + "}), ({:" + tag + "}, {:" + tag + "}), ({:" + tag + "}, {:" + tag +	"}))");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y, value.p3.x, value.p3.y));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Bezier3, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Bezier3, s3d::char32>::format(const s3d::Bezier3& value, s3d::BufferContext& ctx) const
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

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
# include <Siv3D/FloatFormatter.hpp>

namespace s3d
{
	namespace
	{
		constexpr double GL5_X[5] =
		{
			0.0,
			-0.5384693101056831,  0.5384693101056831,
			-0.9061798459386640,  0.9061798459386640
		};

		constexpr double GL5_W[5] =
		{
			0.5688888888888889,
			0.4786286704993665,  0.4786286704993665,
			0.2369268850561891,  0.2369268850561891
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	getTangent
	//
	////////////////////////////////////////////////////////////////

	Vec2 Bezier3::getTangent(const double t) const noexcept
	{
		return getDerivative(t).normalized();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getNormal
	//
	////////////////////////////////////////////////////////////////

	Vec2 Bezier3::getNormal(const double t) const noexcept
	{
		const Vec2 d = getDerivative(t);
		return Vec2{ d.y, -d.x }.normalized();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCurvature
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::getCurvature(const double t) const noexcept
	{
		const Vec2 d1 = getDerivative(t);          // B'(t)
		const Vec2 d2 = getSecondDerivative(t);    // B''(t)

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
	//	getCurvatureRadius
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::getCurvatureRadius(const double t) const noexcept
	{
		const double curvature = getCurvature(t);

		if (curvature == 0.0)
		{
			return Math::Inf;
		}

		return (1.0 / Abs(curvature));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getHeading
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::getHeading(const double t) const noexcept
	{
		return getDerivative(t).getAngle();
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeLength
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::computeLength() const noexcept
	{
		constexpr int32 N = 32;
		double sum = 0.0;

		for (int32 i = 0; i < N; ++i)
		{
			const double a = (static_cast<double>(i) / N);
			const double b = (static_cast<double>(i + 1) / N);
			sum += integrateSpeed(a, b);
		}

		return sum;
	}

	////////////////////////////////////////////////////////////////
	//
	//	tAtLength
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::tAtLength(const double s) const noexcept
	{
		constexpr int32 N = 32;
		constexpr double invN = (1.0 / static_cast<double>(N));

		// 端の扱い
		if (s <= 0.0)
		{
			return 0.0;
		}

		// 累積弧長テーブル
		double cum[N + 1]{};

		for (int32 i = 0; i < N; ++i)
		{
			const double a = (static_cast<double>(i) * invN);
			const double b = (static_cast<double>(i + 1) * invN);
			cum[i + 1] = (cum[i] + integrateSpeed(a, b));
		}

		const double total = cum[N];
		if (!(total > 0.0))
		{
			return 0.0; // 退化（ほぼ長さ 0）
		}

		if (s >= total)
		{
			return 1.0;
		}

		// s が入る区間を探す（cum[i] <= s < cum[i+1]）
		int32 seg = 0;
		{
			int32 lo = 0, hi = N;
			while (lo < hi)
			{
				const int32 mid = (lo + hi) >> 1;
				if (cum[mid] < s) lo = mid + 1;
				else              hi = mid;
			}
			seg = (lo > 0) ? (lo - 1) : 0;
			if (seg >= N) seg = N - 1;
		}

		const double a = (static_cast<double>(seg) * invN);
		const double b = (static_cast<double>(seg + 1) * invN);
		const double segLen = (cum[seg + 1] - cum[seg]);

		// 初期推定（区間内線形補間）
		double t = (0.5 * (a + b));
		if (0.0 < segLen)
		{
			const double u = ((s - cum[seg]) / segLen); // 0..1
			t = (a + (b - a) * Clamp(u, 0.0, 1.0));
		}

		// 収束条件（相対＋絶対）
		const double tol = (1e-6 * total + 1e-9);

		// bracket
		double loT = a;
		double hiT = b;

		// 保証付き Newton（区間外に出たら二分へフォールバック）
		constexpr int32 maxIter = 12;
		constexpr double fpEps = 1e-12;

		for (int32 it = 0; it < maxIter; ++it)
		{
			t = Clamp(t, loT, hiT);

			// L(t) = cum[seg] + ∫[a,t] |B'(u)| du
			const double Lt = (cum[seg] + integrateSpeed(a, t));
			const double f = (Lt - s);

			if (Abs(f) <= tol)
			{
				return t;
			}

			// bracket update
			if (f < 0.0) loT = t;
			else         hiT = t;

			const double fp = speed(t);

			double tNew = (0.5 * (loT + hiT)); // fallback
			if (fpEps < fp)
			{
				tNew = t - (f / fp);
				if (!(tNew > loT && tNew < hiT))
				{
					tNew = (0.5 * (loT + hiT));
				}
			}

			t = tNew;
		}

		return Clamp(t, a, b);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPosAtLength
	//
	////////////////////////////////////////////////////////////////

	Bezier3::position_type Bezier3::getPosAtLength(const double length) const noexcept
	{
		return getPos(tAtLength(length));
	}

	////////////////////////////////////////////////////////////////
	//
	//	closestT
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::closestT(const position_type& point) const noexcept
	{
		constexpr double Eps = 1e-12;
		constexpr double DegenerateRel = 1e-24; // relative threshold

		// 退化: (ほぼ)直線 → 線分 [p0, p3] への射影
		{
			const Vec2 chord = (p3 - p0);
			const double chordLenSq = chord.dot(chord);

			if (chordLenSq <= Eps)
			{
				return 0.0; // ほぼ一点
			}

			const double scale = Max(1.0, chordLenSq);

			// p1, p2 が chord にほぼ乗っているか（面積 = cross）
			const double c1 = (p1 - p0).cross(chord);
			const double c2 = (p2 - p0).cross(chord);

			// cross^2 を length^4 に対して相対比較
			if ((c1 * c1 <= (DegenerateRel * scale * scale))
				&& (c2 * c2 <= (DegenerateRel * scale * scale)))
			{
				const double t = ((point - p0).dot(chord) / chordLenSq);
				return Clamp(t, 0.0, 1.0);
			}
		}

		// 端点を初期最良に
		double bestT = 0.0;
		double bestDistSq = point.distanceFromSq(p0);

		{
			const double d2 = point.distanceFromSq(p3);
			if (d2 < bestDistSq)
			{
				bestDistSq = d2;
				bestT = 1.0;
			}
		}

		// D'(t)=0 を解く（数値）
		// f(t) = (B(t)-P)·B'(t)
		// f'(t)= B'(t)·B'(t) + (B(t)-P)·B''(t)
		auto f = [this, &point](double t) noexcept -> double
		{
			const Vec2 r = (getPos(t) - point);
			const Vec2 d = getDerivative(t);
			return r.dot(d);
		};

		auto fp = [this, &point](double t) noexcept -> double
		{
			const Vec2 r = (getPos(t) - point);
			const Vec2 d1 = getDerivative(t);
			const Vec2 d2 = getSecondDerivative(t);
			return (d1.dot(d1) + r.dot(d2));
		};

		auto consider = [this, &point, &bestT, &bestDistSq](double t) noexcept
		{
			t = Clamp(t, 0.0, 1.0);
			const double d2 = point.distanceFromSq(getPos(t));
			if (d2 < bestDistSq)
			{
				bestDistSq = d2;
				bestT = t;
			}
		};

		// 粗いサンプリングで bracket を作り、保証付き Newton で収束
		constexpr int32 S = 64; // 分割数
		constexpr int32 maxIter = 16;
		constexpr double fpEps = 1e-14;
		constexpr double tolF = 1e-12;

		double tPrev = 0.0;
		double fPrev = f(tPrev);

		consider(0.0);

		for (int32 i = 1; i <= S; ++i)
		{
			const double tCur = (static_cast<double>(i) / S);
			const double fCur = f(tCur);

			consider(tCur);

			// sign change（または端のゼロ）を bracket として解く
			const bool bracket =
				((fPrev <= 0.0 && 0.0 <= fCur) || (fCur <= 0.0 && 0.0 <= fPrev));

			if (bracket)
			{
				double lo = tPrev;
				double hi = tCur;
				double flo = fPrev;
				double fhi = fCur;

				// どちらかがほぼ 0 ならその点を候補に
				if (Abs(flo) <= tolF) { consider(lo); }
				if (Abs(fhi) <= tolF) { consider(hi); }

				// bracket が有効なら refine
				if (lo < hi && !(Abs(flo) <= tolF && Abs(fhi) <= tolF))
				{
					double t = (0.5 * (lo + hi));

					for (int32 it = 0; it < maxIter; ++it)
					{
						t = Clamp(t, lo, hi);

						const double ft = f(t);
						if (Abs(ft) <= tolF)
						{
							break;
						}

						// bracket 更新
						if ((flo <= 0.0 && 0.0 <= ft) || (ft <= 0.0 && 0.0 <= flo))
						{
							hi = t;
							fhi = ft;
						}
						else
						{
							lo = t;
							flo = ft;
						}

						double tNew = (0.5 * (lo + hi)); // fallback: bisection
						const double fpt = fp(t);

						if (fpEps < Abs(fpt))
						{
							const double cand = (t - ft / fpt);
							if (cand > lo && cand < hi)
							{
								tNew = cand;
							}
						}

						t = tNew;
					}

					consider(t);
				}
			}

			tPrev = tCur;
			fPrev = fCur;
		}

		return bestT; // [0,1]
	}

	////////////////////////////////////////////////////////////////
	//
	//  closestPoint
	//
	////////////////////////////////////////////////////////////////

	Bezier3::position_type Bezier3::closestPoint(const position_type& point) const noexcept
	{
		return getPos(closestT(point));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLineString
	//
	////////////////////////////////////////////////////////////////

	LineString Bezier3::getLineString(int32 segments) const
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
				*pDst++ = getPos(t);
			}

			// 終点
			*pDst = p3;
		}

		return pts;
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
			const Vec2 p = getPos(t0);
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
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Bezier3& Bezier3::draw(const ColorF& color, const int32 segments) const
	{
		getLineString(segments).draw(color);
		return *this;
	}

	const Bezier3& Bezier3::draw(const double thickness, const ColorF& color, const int32 segments) const
	{
		getLineString(segments).draw(thickness, color);
		return *this;
	}

	const Bezier3& Bezier3::draw(const LineCap linaCap, const double thickness, const ColorF& color, const int32 segments) const
	{
		getLineString(segments).draw(linaCap, thickness, color);
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

	double Bezier3::speed(double t) const noexcept
	{
		t = Clamp(t, 0.0, 1.0);
		return getDerivative(t).length();
	}

	double Bezier3::integrateSpeed(double a, double b) const noexcept
	{
		a = Clamp(a, 0.0, 1.0);
		b = Clamp(b, 0.0, 1.0);
		if (b <= a)
		{
			return 0.0;
		}

		const double m = (0.5 * (a + b));
		const double h = (0.5 * (b - a));

		double local = 0.0;
		for (int32 k = 0; k < 5; ++k)
		{
			local += GL5_W[k] * speed(m + h * GL5_X[k]);
		}

		return (h * local);
	}

	void Bezier3::ThrowPointAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Bezier3::pointAtIndex() index out of range" };
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Bezier3>::format(const s3d::Bezier3& value, fmt::format_context& ctx)
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

s3d::BufferContext::iterator fmt::formatter<s3d::Bezier3, s3d::char32>::format(const s3d::Bezier3& value, s3d::BufferContext& ctx)
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

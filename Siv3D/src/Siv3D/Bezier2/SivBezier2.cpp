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
# include <Siv3D/ListUtility.hpp>
# include <Siv3D/PolynomialSolver.hpp>
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

	Vec2 Bezier2::getTangent(double t) const noexcept
	{
		const Vec2 v0 = (p1 - p0);
		const Vec2 v1 = (p2 - p1);
		const Vec2 d = (v0 * (1.0 - t) + v1 * t);
		return d.normalized();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getNormal
	//
	////////////////////////////////////////////////////////////////

	Vec2 Bezier2::getNormal(double t) const noexcept
	{
		const Vec2 d = getDerivative(t);
		return Vec2{ d.y, -d.x }.normalized();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getHeading
	//
	////////////////////////////////////////////////////////////////

	double Bezier2::getHeading(const double t) const noexcept
	{
		return getDerivative(t).getAngle();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getCurvature
	//
	////////////////////////////////////////////////////////////////

	double Bezier2::getCurvature(const double t) const noexcept
	{
		const Vec2 d1 = getDerivative(t);        // B'(t)
		const Vec2 d2 = getSecondDerivative();   // B''(t)（定数）

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

	double Bezier2::getCurvatureRadius(const double t) const noexcept
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
	//	computeLength
	//
	////////////////////////////////////////////////////////////////

	double Bezier2::computeLength() const noexcept
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

	double Bezier2::tAtLength(const double s) const noexcept
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

	Bezier2::position_type Bezier2::getPosAtLength(const double length) const noexcept
	{
		return getPos(tAtLength(length));
	}

	////////////////////////////////////////////////////////////////
	//
	//  closestT
	//
	////////////////////////////////////////////////////////////////

	double Bezier2::closestT(const position_type& point) const noexcept
	{
		constexpr double Eps = 1e-12;
		constexpr double DegenerateRel = 1e-24; // relative threshold for M.dot(M)

		const Vec2 M = (p0 - 2.0 * p1 + p2);
		const Vec2 N = (p1 - p0);
		const Vec2 K = (point - p0);

		// 退化: (ほぼ)直線 → 線分 [p0, p2] への射影
		{
			const Vec2 chord = (p2 - p0);
			const double chordLenSq = chord.dot(chord);
			const double scale = Max(1.0, chordLenSq);

			if (M.dot(M) <= (DegenerateRel * scale))
			{
				if (chordLenSq <= Eps)
				{
					return 0.0; // ほぼ一点
				}

				const double t = (K.dot(chord) / chordLenSq);
				return Clamp(t, 0.0, 1.0);
			}
		}

		// 端点を初期最良に
		double bestT = 0.0;
		double bestDistSq = point.distanceFromSq(p0);

		{
			const double d2 = point.distanceFromSq(p2);
			if (d2 < bestDistSq)
			{
				bestDistSq = d2;
				bestT = 1.0;
			}
		}

		// D'(t)=0 の 3次方程式:
		// (M·M)t^3 + 3(M·N)t^2 + (2N·N - M·K)t - N·K = 0
		const double c3 = M.dot(M);
		const double c2 = 3.0 * M.dot(N);
		const double c1 = (2.0 * N.dot(N) - M.dot(K));
		const double c0 = -N.dot(K);

		if (const auto opt = Math::SolveCubicEquation(c3, c2, c1, c0))
		{
			for (const double x : *opt)
			{
				if (InRange(x, -Eps, (1.0 + Eps)))
				{
					const double t = Clamp(x, 0.0, 1.0);
					const double d2 = point.distanceFromSq(getPos(t));
					if (d2 < bestDistSq)
					{
						bestDistSq = d2;
						bestT = t;
					}
				}
			}
		}

		return bestT; // 既に [0,1]
	}

	////////////////////////////////////////////////////////////////
	//
	//  closestPoint
	//
	////////////////////////////////////////////////////////////////

	Bezier2::position_type Bezier2::closestPoint(const position_type& point) const noexcept
	{
		return getPos(closestT(point));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLineString
	//
	////////////////////////////////////////////////////////////////

	LineString Bezier2::getLineString(int32 segments) const
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
			*pDst = p2;
		}

		return pts;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLineStringAdaptive
	//
	////////////////////////////////////////////////////////////////

	LineString Bezier2::getLineStringAdaptive(const double maxError, const int32 maxDepth) const
	{
		LineString polyline;
		polyline.reserve(33);

		polyline.push_back(p0);
		Vec2 lastPushed = p0;

		const double kNearlyZeroSq = 1e-12;
		const double maxErrorSq = (maxError * maxError);
		const int32  depthLimit = Max(0, maxDepth);

		// 平坦度判定の閾値（ (cross^2) <= (4*error^2*|chord|^2) ）
		const double flatnessK = (4.0 * maxErrorSq);

		// 極短区間（弦＋制御多角形）に対する早期終了の閾値
		const double shortSegmentSq = (maxErrorSq * 0.25);

		// 直前に追加した点と同一なら追加しない（重複点の抑制）
		auto PushUnique = [&](const Vec2& p)
		{
			if (lastPushed != p)
			{
				polyline.push_back(p);
				lastPushed = p;
			}
		};

		auto SubdivideFlatten = [&](auto&& self, const Bezier2& curve, const int32 depth) -> void
		{
			const Vec2   chord = (curve.p2 - curve.p0);
			const double chordLenSq = chord.lengthSq();

			// ケースA: 弦がほぼゼロ長（p0 ≒ p2）
			// p1 が離れているなら「退化」ではないので分割する。
			// 3点ともほぼ同一点なら退化として扱う。
			if (chordLenSq < kNearlyZeroSq)
			{
				const double p0p1LenSq = (curve.p1 - curve.p0).lengthSq();
				const double p1p2LenSq = (curve.p2 - curve.p1).lengthSq();
				const double ctrlSpanSq = Max(p0p1LenSq, p1p2LenSq);

				// 3点ともほぼ同一点なら退化
				if (ctrlSpanSq < kNearlyZeroSq)
				{
					PushUnique(curve.p2);
					return;
				}

				// 深さ上限に達しているなら p1 を残して形が潰れないようにする
				if (depthLimit <= depth)
				{
					PushUnique(curve.p1);
					PushUnique(curve.p2);
					return;
				}

				// 弦がほぼゼロなので平坦度判定は不安定：とにかく分割
				const Vec2 p01 = ((curve.p0 + curve.p1) * 0.5);
				const Vec2 p12 = ((curve.p1 + curve.p2) * 0.5);
				const Vec2 p012 = ((p01 + p12) * 0.5);

				self(self, Bezier2{ curve.p0,  p01,  p012 }, (depth + 1));
				self(self, Bezier2{ p012,      p12,  curve.p2 }, (depth + 1));
				return;
			}

			// ケースB: 通常の平坦度判定（p1 の弦からのズレを外積で評価）
			const Vec2   v = (curve.p1 - curve.p0);
			const double cross = chord.cross(v);

			// |cross| は |chord|×(弦からの距離) に比例するため、二乗で比較する
			bool acceptSegment = ((cross * cross) <= (flatnessK * chordLenSq));

			if (acceptSegment)
			{
				// p1 の射影が弦区間 [p0,p2] に収まることを確認（折り返し/ループ対策）
				const double dot = v.dot(chord);
				if ((dot < 0.0) || (chordLenSq < dot))
				{
					acceptSegment = false;
				}
			}

			// 平坦とみなせる、または深さ上限に達しているなら p2 を採用
			if (acceptSegment || (depthLimit <= depth))
			{
				PushUnique(curve.p2);
				return;
			}

			// 追加の早期終了：
			// 弦だけでなく制御多角形（p0-p1, p1-p2）も極短ならこれ以上分割しない
			const double p0p1LenSq = (curve.p1 - curve.p0).lengthSq();
			const double p1p2LenSq = (curve.p2 - curve.p1).lengthSq();
			if ((chordLenSq <= shortSegmentSq) && (p0p1LenSq <= shortSegmentSq) && (p1p2LenSq <= shortSegmentSq))
			{
				PushUnique(curve.p2);
				return;
			}

			// 分割（De Casteljau 法で t=0.5 の中点分割）
			const Vec2 p01 = ((curve.p0 + curve.p1) * 0.5);
			const Vec2 p12 = ((curve.p1 + curve.p2) * 0.5);
			const Vec2 p012 = ((p01 + p12) * 0.5);

			self(self, Bezier2{ curve.p0,  p01,  p012 }, (depth + 1));
			self(self, Bezier2{ p012,      p12,  curve.p2 }, (depth + 1));
		};

		SubdivideFlatten(SubdivideFlatten, *this, 0);
		return polyline;
	}

	////////////////////////////////////////////////////////////////
	//
	//	subcurve
	//
	////////////////////////////////////////////////////////////////

	Bezier2 Bezier2::subcurve(double t0, double t1) const noexcept
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
			return{ p, p, p };
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
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	RectF Bezier2::boundingRect() const noexcept
	{
		// 許容誤差の設定
		constexpr double Eps = (64.0 * std::numeric_limits<double>::epsilon());
		constexpr double tTol = (16.0 * Eps);

		auto Update1D = [](double p0, double p1, double p2, double& mn, double& mx) noexcept
		{
			// 多項式 P(t) = A t^2 + B t + C
			const double A = (p0 - 2.0 * p1 + p2);
			const double B = (2.0 * (p1 - p0));
			const double C = p0;

			// まず端点で初期化
			mn = Min(p0, p2);
			mx = Max(p0, p2);

			// 評価関数: P(t) の計算 (Horner's method with FMA)
			auto Eval = [&](double t) noexcept
			{
				return std::fma(std::fma(A, t, B), t, C);
			};

			// 候補 t の採用判定
			auto ConsiderT = [&](double t) noexcept
			{
				// 境界判定を少し緩め、採用する場合は [0,1] に clamp
				if (InRange(t, -tTol, (1.0 + tTol)))
				{
					t = Clamp(t, 0.0, 1.0);
					const double v = Eval(t);
					mn = Min(mn, v);
					mx = Max(mx, v);
				}
			};

			// 係数のスケール（微小値判定用）
			const double scale = Max({ Abs(p0), Abs(p1), Abs(p2), 1.0 });

			// 導関数 P'(t) = 2A t + B = 0 を解く
			// A が極小の場合は線形とみなす（極値なし、端点のみ）
			if (Abs(A) <= (Eps * scale))
			{
				return;
			}

			// t = -B / 2A
			ConsiderT(-B / (2.0 * A));
		};

		double minX, maxX, minY, maxY;
		Update1D(p0.x, p1.x, p2.x, minX, maxX);
		Update1D(p0.y, p1.y, p2.y, minY, maxY);
		return{ minX, minY, (maxX - minX), (maxY - minY) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Bezier2& Bezier2::draw(const ColorF& color, const int32 segments) const
	{
		return draw(1.0, color, segments);
	}

	const Bezier2& Bezier2::draw(double thickness, const ColorF& color, const int32 segments) const
	{
		getLineString(segments).draw(thickness, color);
		return *this;
	}

	const Bezier2& Bezier2::draw(const LineCap linaCap, const double thickness, const ColorF& color, const int32 segments) const
	{
		getLineString(segments).draw(linaCap, thickness, color);
		return *this;
	}

	const Bezier2& Bezier2::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& color, const int32 segments) const
	{
		getLineString(segments).draw(startCap, endCap, thickness, color);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawAdaptive
	//
	////////////////////////////////////////////////////////////////

	const Bezier2& Bezier2::drawAdaptive(const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		return drawAdaptive(1.0, color, maxError, maxDepth);
	}

	const Bezier2& Bezier2::drawAdaptive(double thickness, const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		getLineStringAdaptive(maxError, maxDepth).draw(thickness, color);
		return *this;
	}

	const Bezier2& Bezier2::drawAdaptive(LineCap lineCap, double thickness, const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		getLineStringAdaptive(maxError, maxDepth).draw(lineCap, thickness, color);
		return *this;
	}

	const Bezier2& Bezier2::drawAdaptive(LineCap startCap, LineCap endCap, double thickness, const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		getLineStringAdaptive(maxError, maxDepth).draw(startCap, endCap, thickness, color);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Bezier2& value)
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
		formatData.string.append(U"))"_sv);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	double Bezier2::speed(double t) const noexcept
	{
		t = Clamp(t, 0.0, 1.0);
		return getDerivative(t).length();
	}

	double Bezier2::integrateSpeed(double a, double b) const noexcept
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

	void Bezier2::ThrowPointAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Bezier2::pointAtIndex() index out of range" };
	}
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

fmt::format_context::iterator fmt::formatter<s3d::Bezier2>::format(const s3d::Bezier2& value, fmt::format_context& ctx)
{
	if (tag.empty())
	{
		return fmt::format_to(ctx.out(), "(({}, {}), ({}, {}), ({}, {}))", value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y);
	}
	else
	{
		const std::string format
			= ("(({:" + tag + "}, {:" + tag + "}), ({:" + tag + "}, {:" + tag + "}), ({:" + tag + "}, {:" + tag + "}))");
		return fmt::vformat_to(ctx.out(), format, fmt::make_format_args(value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y));
	}
}

s3d::ParseContext::iterator fmt::formatter<s3d::Bezier2, s3d::char32>::parse(s3d::ParseContext& ctx)
{
	return s3d::FmtHelper::GetFormatTag(tag, ctx);
}

s3d::BufferContext::iterator fmt::formatter<s3d::Bezier2, s3d::char32>::format(const s3d::Bezier2& value, s3d::BufferContext& ctx)
{
	if (tag.empty())
	{
		return format_to(ctx.out(), U"(({}, {}), ({}, {}), ({}, {}))", value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y);
	}
	else
	{
		const std::u32string format
			= (U"(({:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}), ({:" + tag + U"}, {:" + tag + U"}))");
		return format_to(ctx.out(), format, value.p0.x, value.p0.y, value.p1.x, value.p1.y, value.p2.x, value.p2.y);
	}
}

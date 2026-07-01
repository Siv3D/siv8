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
	//	computeTAtDistance
	//
	////////////////////////////////////////////////////////////////

	double Bezier3::computeTAtDistance(const double distanceFromStart) const noexcept
	{
		constexpr int32 N = 32;
		constexpr double invN = (1.0 / static_cast<double>(N));

		// 端の扱い
		if (distanceFromStart <= 0.0)
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

		if (distanceFromStart >= total)
		{
			return 1.0;
		}

		// distanceFromStart が入る区間を探す（cum[i] <= s < cum[i+1]）
		int32 seg = 0;
		{
			int32 lo = 0, hi = N;
			while (lo < hi)
			{
				const int32 mid = (lo + hi) >> 1;
				if (cum[mid] < distanceFromStart) lo = mid + 1;
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
			const double u = ((distanceFromStart - cum[seg]) / segLen); // 0..1
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
			const double f = (Lt - distanceFromStart);

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
				const double t = ((targetPoint - p0).dot(chord) / chordLenSq);
				return Clamp(t, 0.0, 1.0);
			}
		}

		// 端点を初期最良に
		double bestT = 0.0;
		double bestDistSq = targetPoint.distanceFromSq(p0);

		{
			const double d2 = targetPoint.distanceFromSq(p3);
			if (d2 < bestDistSq)
			{
				bestDistSq = d2;
				bestT = 1.0;
			}
		}

		// D'(t)=0 を解く（数値）
		// f(t) = (B(t)-P)·B'(t)
		// f'(t)= B'(t)·B'(t) + (B(t)-P)·B''(t)
		auto f = [this, &targetPoint](double t) noexcept -> double
		{
			const Vec2 r = (pointAt(t) - targetPoint);
			const Vec2 d = derivativeAt(t);
			return r.dot(d);
		};

		auto fp = [this, &targetPoint](double t) noexcept -> double
		{
			const Vec2 r = (pointAt(t) - targetPoint);
			const Vec2 d1 = derivativeAt(t);
			const Vec2 d2 = secondDerivativeAt(t);
			return (d1.dot(d1) + r.dot(d2));
		};

		auto consider = [this, &targetPoint, &bestT, &bestDistSq](double t) noexcept
		{
			t = Clamp(t, 0.0, 1.0);
			const double d2 = targetPoint.distanceFromSq(pointAt(t));
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
	//  computeClosestPoint
	//
	////////////////////////////////////////////////////////////////

	Bezier3::position_type Bezier3::computeClosestPoint(const position_type& targetPoint) const noexcept
	{
		return pointAt(computeClosestT(targetPoint));
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
				*pDst++ = pointAt(t);
			}

			// 終点
			*pDst = p3;
		}

		return pts;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getLineStringAdaptive
	//
	////////////////////////////////////////////////////////////////

	LineString Bezier3::getLineStringAdaptive(const double maxError, const int32 maxDepth) const
	{
		LineString polyline;
		polyline.reserve(65);

		polyline.push_back(p0);
		Vec2 lastPushed = p0;

		const double kNearlyZeroSq = 1e-12;
		const double maxErrorSq = (maxError * maxError);
		const int32  depthLimit = Max(0, maxDepth);

		// 平坦度判定の閾値（ (cross^2) <= (error^2*|chord|^2) ）
		// ※ Bezier2 版は係数 4 を使っているが、Bezier3 は制御点が2つなので安全側に 4 を維持
		const double flatnessK = (4.0 * maxErrorSq);

		// 極短区間の早期終了
		const double shortSegmentSq = (maxErrorSq * 0.25);

		auto PushUnique = [&](const Vec2& p)
		{
			if (lastPushed != p)
			{
				polyline.push_back(p);
				lastPushed = p;
			}
		};

		auto SubdivideFlatten = [&](auto&& self, const Bezier3& curve, const int32 depth) -> void
		{
			const Vec2   chord = (curve.p3 - curve.p0);
			const double chordLenSq = chord.lengthSq();

			// ケースA: 弦がほぼゼロ長（p0 ≒ p3）
			if (chordLenSq < kNearlyZeroSq)
			{
				const double p0p1LenSq = (curve.p1 - curve.p0).lengthSq();
				const double p1p2LenSq = (curve.p2 - curve.p1).lengthSq();
				const double p2p3LenSq = (curve.p3 - curve.p2).lengthSq();
				const double ctrlSpanSq = Max({ p0p1LenSq, p1p2LenSq, p2p3LenSq });

				// 4点ともほぼ同一点なら退化
				if (ctrlSpanSq < kNearlyZeroSq)
				{
					PushUnique(curve.p3);
					return;
				}

				// 深さ上限に達しているなら制御点を残して潰れを防ぐ
				if (depthLimit <= depth)
				{
					PushUnique(curve.p1);
					PushUnique(curve.p2);
					PushUnique(curve.p3);
					return;
				}

				// 弦がほぼゼロなので平坦度判定は不安定：とにかく分割（t=0.5）
				const Vec2 p01 = ((curve.p0 + curve.p1) * 0.5);
				const Vec2 p12 = ((curve.p1 + curve.p2) * 0.5);
				const Vec2 p23 = ((curve.p2 + curve.p3) * 0.5);
				const Vec2 p012 = ((p01 + p12) * 0.5);
				const Vec2 p123 = ((p12 + p23) * 0.5);
				const Vec2 p0123 = ((p012 + p123) * 0.5);

				self(self, Bezier3{ curve.p0, p01,  p012,  p0123 }, (depth + 1));
				self(self, Bezier3{ p0123,    p123, p23,   curve.p3 }, (depth + 1));
				return;
			}

			// ケースB: 通常の平坦度判定
			// p1, p2 の弦からのズレを cross で評価（最大のものを採用）
			const Vec2 v1 = (curve.p1 - curve.p0);
			const Vec2 v2 = (curve.p2 - curve.p0);

			const double cross1 = chord.cross(v1);
			const double cross2 = chord.cross(v2);

			double maxCrossAbsSq = (cross1 * cross1);
			maxCrossAbsSq = Max(maxCrossAbsSq, (cross2 * cross2));

			bool acceptSegment = (maxCrossAbsSq <= (flatnessK * chordLenSq));

			if (acceptSegment)
			{
				// 両制御点の射影が弦区間 [p0,p3] に収まることを確認（折り返し/ループ対策）
				const double dot1 = v1.dot(chord);
				const double dot2 = v2.dot(chord);

				if ((dot1 < 0.0) || (chordLenSq < dot1) || (dot2 < 0.0) || (chordLenSq < dot2))
				{
					acceptSegment = false;
				}
			}

			// 平坦とみなせる、または深さ上限に達しているなら p3 を採用
			if (acceptSegment || (depthLimit <= depth))
			{
				PushUnique(curve.p3);
				return;
			}

			// 追加の早期終了：弦＋制御多角形が極短ならこれ以上分割しない
			const double p0p1LenSq = (curve.p1 - curve.p0).lengthSq();
			const double p1p2LenSq = (curve.p2 - curve.p1).lengthSq();
			const double p2p3LenSq = (curve.p3 - curve.p2).lengthSq();

			if ((chordLenSq <= shortSegmentSq)
				&& (p0p1LenSq <= shortSegmentSq)
				&& (p1p2LenSq <= shortSegmentSq)
				&& (p2p3LenSq <= shortSegmentSq))
			{
				PushUnique(curve.p3);
				return;
			}

			// 分割（De Casteljau 法で t=0.5 の中点分割）
			const Vec2 p01 = ((curve.p0 + curve.p1) * 0.5);
			const Vec2 p12 = ((curve.p1 + curve.p2) * 0.5);
			const Vec2 p23 = ((curve.p2 + curve.p3) * 0.5);
			const Vec2 p012 = ((p01 + p12) * 0.5);
			const Vec2 p123 = ((p12 + p23) * 0.5);
			const Vec2 p0123 = ((p012 + p123) * 0.5);

			self(self, Bezier3{ curve.p0, p01,  p012,  p0123 }, (depth + 1));
			self(self, Bezier3{ p0123,    p123, p23,   curve.p3 }, (depth + 1));
		};

		SubdivideFlatten(SubdivideFlatten, *this, 0);
		return polyline;
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
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	RectF Bezier3::boundingRect() const noexcept
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
	//	inflectionTs
	//
	////////////////////////////////////////////////////////////////

	Array<double> Bezier3::inflectionTs() const
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
		getLineStringAdaptive().paint(dst, lineCap, thickness, color, enableAntialiasing);
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
		getLineStringAdaptive().overwrite(dst, lineCap, thickness, color, enableAntialiasing);
		return *this;
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

	const Bezier3& Bezier3::draw(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& color, const int32 segments) const
	{
		getLineString(segments).draw(startCap, endCap, thickness, color);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawAdaptive
	//
	////////////////////////////////////////////////////////////////

	const Bezier3& Bezier3::drawAdaptive(const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		getLineStringAdaptive(maxError, maxDepth).draw(color);
		return *this;
	}

	const Bezier3& Bezier3::drawAdaptive(const double thickness, const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		getLineStringAdaptive(maxError, maxDepth).draw(thickness, color);
		return *this;
	}

	const Bezier3& Bezier3::drawAdaptive(const LineCap linaCap, const double thickness, const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		getLineStringAdaptive(maxError, maxDepth).draw(linaCap, thickness, color);
		return *this;
	}

	const Bezier3& Bezier3::drawAdaptive(const LineCap startCap, const LineCap endCap, const double thickness, const ColorF& color, const double maxError, const int32 maxDepth) const
	{
		getLineStringAdaptive(maxError, maxDepth).draw(startCap, endCap, thickness, color);
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
		return derivativeAt(t).length();
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

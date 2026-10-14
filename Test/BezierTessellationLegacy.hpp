//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <Siv3D/Bezier.hpp>
# include <Siv3D/LineString.hpp>

// Frozen recursive baselines for reproducible comparison with density tessellation.
// Kept out of production code; the public conversion APIs use the shared kernel.
namespace s3d::BezierStudy::Legacy
{
	inline LineString Flatten(const Bezier2& input, const double maxError, const int32 maxDepth)
	{
		const Vec2 p0 = input.p0;
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

		SubdivideFlatten(SubdivideFlatten, input, 0);
		return polyline;
	}

	inline double Bezier3ControlDistanceSq(const Vec2 point, const Vec2 start, const Vec2 chord, const double chordSq) noexcept
	{
		const double t = ((chordSq == 0.0) ? 0.0 : Clamp((point - start).dot(chord) / chordSq, 0.0, 1.0));
		return point.distanceFromSq(start + t * chord);
	}

	// Append endpoints without allocating a temporary polyline for each curve.
	inline void AppendBezier3Polyline(LineString& destination, const Bezier3& curve, const double errorSq, const int32 depth)
	{
		const Vec2 chord = (curve.p3 - curve.p0);
		const double chordSq = chord.lengthSq();
		if ((depth == 0)
			|| ((Bezier3ControlDistanceSq(curve.p1, curve.p0, chord, chordSq) <= errorSq)
				&& (Bezier3ControlDistanceSq(curve.p2, curve.p0, chord, chordSq) <= errorSq)))
		{
			if (destination.back() != curve.p3)
			{
				destination.push_back(curve.p3);
			}
			return;
		}
		const auto [left, right] = curve.split(0.5);
		AppendBezier3Polyline(destination, left, errorSq, (depth - 1));
		AppendBezier3Polyline(destination, right, errorSq, (depth - 1));
	}
	inline LineString Flatten(const Bezier3& curve, const double maxError, const int32 maxDepth)
	{
		LineString result;
		result.push_back(curve.p0);
		AppendBezier3Polyline(result, curve, maxError * maxError, Max(0, maxDepth));
		return result;
	}
}

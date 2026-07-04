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

# pragma once

namespace s3d
{
	namespace detail
	{
		/// @brief 有向辺 (a → b) に対する点の外積 (b - a) × (point - a) を返します。
		/// @remark Siv3D 座標系(x 右, y 下)では、時計回り多角形の内部の点に対して正になります。
		[[nodiscard]]
		constexpr double EdgeSide(const Vec2& a, const Vec2& b, const Vec2& point) noexcept
		{
			return (((b.x - a.x) * (point.y - a.y)) - ((b.y - a.y) * (point.x - a.x)));
		}

		/// @brief 2 点が異なる座標かを返します。
		/// @remark ゼロ長辺(隣接する同一頂点)の外積は点の位置によらず恒等的に 0 になり、
		///         境界上の証拠にならないため、境界判定から除外するために使用します。
		[[nodiscard]]
		constexpr bool IsDistinct(const Vec2& a, const Vec2& b) noexcept
		{
			return ((a.x != b.x) || (a.y != b.y));
		}

		/// @brief 点が 3 頂点の AABB 内にあるかを返します。
		/// @remark 縮退した(全頂点が同一直線上または同一点の)図形のフォールバック判定用。
		///         共線な頂点集合の凸包は「支持直線 ∩ AABB」に一致するため、点が支持直線上に
		///         あることが確定した文脈では、AABB 包含が縮退図形上の判定と厳密に等価です。
		///         全頂点が同一点の場合は AABB がその一点に一致し、同一点判定になります。
		[[nodiscard]]
		constexpr bool AabbContains(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& point) noexcept
		{
			return ((Min(Min(p0.x, p1.x), p2.x) <= point.x) && (point.x <= Max(Max(p0.x, p1.x), p2.x))
				&& (Min(Min(p0.y, p1.y), p2.y) <= point.y) && (point.y <= Max(Max(p0.y, p1.y), p2.y)));
		}

		/// @brief 点が 4 頂点の AABB 内にあるかを返します。
		/// @remark 3 頂点版と同じく、縮退した図形のフォールバック判定用。
		[[nodiscard]]
		constexpr bool AabbContains(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& point) noexcept
		{
			return ((Min(Min(p0.x, p1.x), Min(p2.x, p3.x)) <= point.x) && (point.x <= Max(Max(p0.x, p1.x), Max(p2.x, p3.x)))
				&& (Min(Min(p0.y, p1.y), Min(p2.y, p3.y)) <= point.y) && (point.y <= Max(Max(p0.y, p1.y), Max(p2.y, p3.y))));
		}

		/// @brief 半平面判定による点包含(3 頂点・縮退許容)
		/// @tparam Boundary 境界上の点の扱い
		/// @tparam IsClockwise 非縮退の頂点列が Siv3D 座標系(x 右, y 下)で時計回りなら true
		/// @remark 非縮退の入力では「全辺で cross >= 0(向き正規化後)」⇔ 点が内部または境界上。
		///         全 cross == 0 となるのは、入力が線分または点に退化している場合に限られます
		///         (正の面積を持つ入力では、実辺の支持直線が共通点を持たないため起こりません)。
		///         線分に退化している場合は点がその支持直線上にあることが確定するため、AABB 包含が
		///         縮退図形上の判定と厳密に等価です。点に退化している場合は任意の点で全 cross == 0
		///         になりますが、AABB 包含がその一点との一致判定になります。
		template <BoundaryPolicy Boundary, bool IsClockwise>
		[[nodiscard]]
		constexpr bool ContainsPointHalfPlane3(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& point) noexcept
		{
			// 反時計回りの場合は符号を反転し、「内側 ⇔ cross >= 0」に正規化する
			constexpr double Orient = (IsClockwise ? 1.0 : -1.0);

			const double c0 = (Orient * EdgeSide(p0, p1, point));
			const double c1 = (Orient * EdgeSide(p1, p2, point));
			const double c2 = (Orient * EdgeSide(p2, p0, point));

			// `not (x >= 0.0)` は `x < 0.0` と数学的に同値だが、NaN 混入時に外部(false)側へ
			// 落ちるため、誤って true を返さない。`x < 0.0` に書き換えないこと。
			// ビット OR は分岐を作らないための意図的な選択(短絡評価は不要)。
			if (static_cast<unsigned>((not (c0 >= 0.0)) | (not (c1 >= 0.0)) | (not (c2 >= 0.0))))
			{
				// 厳密に外部
				return false;
			}

			const unsigned hasPositive = ((c0 > 0.0) | (c1 > 0.0) | (c2 > 0.0));

			if constexpr (Boundary == BoundaryPolicy::Excluded)
			{
				// 境界上 ⇔ 実辺(ゼロ長でない辺)の支持直線上
				const bool onBoundary = (((c0 == 0.0) & IsDistinct(p0, p1))
					| ((c1 == 0.0) & IsDistinct(p1, p2))
					| ((c2 == 0.0) & IsDistinct(p2, p0)));

				// 内部 ⇔ 非縮退(正の cross が存在)かつ境界上でない
				return (hasPositive && (not onBoundary));
			}
			else
			{
				if (hasPositive)
				{
					// 非縮退: 内部または境界上
					return true;
				}

				// 縮退: 図形(線分または点)上か判定
				return AabbContains(p0, p1, p2, point);
			}
		}

		/// @brief 半平面判定による点包含(4 頂点・縮退許容)
		/// @remark 3 頂点版 `ContainsPointHalfPlane3` と同じ方式です。そちらの説明を参照。
		template <BoundaryPolicy Boundary, bool IsClockwise>
		[[nodiscard]]
		constexpr bool ContainsPointHalfPlane4(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& point) noexcept
		{
			constexpr double Orient = (IsClockwise ? 1.0 : -1.0);

			const double c0 = (Orient * EdgeSide(p0, p1, point));
			const double c1 = (Orient * EdgeSide(p1, p2, point));
			const double c2 = (Orient * EdgeSide(p2, p3, point));
			const double c3 = (Orient * EdgeSide(p3, p0, point));

			if (static_cast<unsigned>((not (c0 >= 0.0)) | (not (c1 >= 0.0)) | (not (c2 >= 0.0)) | (not (c3 >= 0.0))))
			{
				return false;
			}

			const unsigned hasPositive = ((c0 > 0.0) | (c1 > 0.0) | (c2 > 0.0) | (c3 > 0.0));

			if constexpr (Boundary == BoundaryPolicy::Excluded)
			{
				const bool onBoundary = (((c0 == 0.0) & IsDistinct(p0, p1))
					| ((c1 == 0.0) & IsDistinct(p1, p2))
					| ((c2 == 0.0) & IsDistinct(p2, p3))
					| ((c3 == 0.0) & IsDistinct(p3, p0)));

				return (hasPositive && (not onBoundary));
			}
			else
			{
				if (hasPositive)
				{
					return true;
				}

				return AabbContains(p0, p1, p2, p3, point);
			}
		}

		/// @brief 外積符号の一貫性判定による点包含(3 頂点・巻き方向不問・縮退許容)
		/// @tparam Boundary 境界上の点の扱い
		/// @remark 全辺の cross の符号が(0 を除いて)一貫していれば、点は内部または境界上。
		///         両符号が現れれば厳密に外部です。全 cross == 0 となるのは、入力が線分または
		///         点に退化している場合に限られ、AABB 包含で縮退図形上かを判定します(線分に
		///         退化している場合、支持直線上にない外部の点は両符号を生じて先に除外されます)。
		template <BoundaryPolicy Boundary>
		[[nodiscard]]
		constexpr bool ContainsPointSignConsistency3(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& point) noexcept
		{
			const double c0 = EdgeSide(p0, p1, point);
			const double c1 = EdgeSide(p1, p2, point);
			const double c2 = EdgeSide(p2, p0, point);

			const unsigned hasPositive = ((c0 > 0.0) | (c1 > 0.0) | (c2 > 0.0));
			const unsigned hasNegative = ((c0 < 0.0) | (c1 < 0.0) | (c2 < 0.0));

			if (hasPositive && hasNegative)
			{
				// 厳密に外部
				return false;
			}

			if constexpr (Boundary == BoundaryPolicy::Excluded)
			{
				// 境界上 ⇔ 実辺(ゼロ長でない辺)の支持直線上
				const bool onBoundary = (((c0 == 0.0) & IsDistinct(p0, p1))
					| ((c1 == 0.0) & IsDistinct(p1, p2))
					| ((c2 == 0.0) & IsDistinct(p2, p0)));

				// 内部 ⇔ 非縮退(いずれかの符号が存在)かつ境界上でない
				return ((hasPositive | hasNegative) && (not onBoundary));
			}
			else
			{
				if (hasPositive | hasNegative)
				{
					// 非縮退: 内部または境界上
					return true;
				}

				// 縮退: 図形(線分または点)上か判定
				return AabbContains(p0, p1, p2, point);
			}
		}

		/// @brief 外積符号の一貫性判定による点包含(4 頂点・巻き方向不問・縮退許容)
		/// @remark 3 頂点版 `ContainsPointSignConsistency3` と同じ方式です。そちらの説明を参照。
		template <BoundaryPolicy Boundary>
		[[nodiscard]]
		constexpr bool ContainsPointSignConsistency4(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& point) noexcept
		{
			const double c0 = EdgeSide(p0, p1, point);
			const double c1 = EdgeSide(p1, p2, point);
			const double c2 = EdgeSide(p2, p3, point);
			const double c3 = EdgeSide(p3, p0, point);

			const unsigned hasPositive = ((c0 > 0.0) | (c1 > 0.0) | (c2 > 0.0) | (c3 > 0.0));
			const unsigned hasNegative = ((c0 < 0.0) | (c1 < 0.0) | (c2 < 0.0) | (c3 < 0.0));

			if (hasPositive && hasNegative)
			{
				return false;
			}

			if constexpr (Boundary == BoundaryPolicy::Excluded)
			{
				const bool onBoundary = (((c0 == 0.0) & IsDistinct(p0, p1))
					| ((c1 == 0.0) & IsDistinct(p1, p2))
					| ((c2 == 0.0) & IsDistinct(p2, p3))
					| ((c3 == 0.0) & IsDistinct(p3, p0)));

				return ((hasPositive | hasNegative) && (not onBoundary));
			}
			else
			{
				if (hasPositive | hasNegative)
				{
					return true;
				}

				return AabbContains(p0, p1, p2, p3, point);
			}
		}

		/// @brief ray casting の 1 辺分の処理
		/// @param a 辺の始点
		/// @param b 辺の終点
		/// @param aAbove `(a.y > point.y)` の事前計算値
		/// @param bAbove `(b.y > point.y)` の事前計算値
		/// @remark 辺 (a → b) と、点から +x 方向に伸びる水平半直線との交差で `inside` を反転
		///         します。交差は y の半開区間で数え、頂点通過の二重カウントを防ぎます。
		///         `Boundary` が `Unspecified` 以外の場合、点が辺上にあれば `onBoundary` を
		///         立てます(辺上 ⇔ 支持直線上(cross == 0)かつ辺の AABB 内)。
		template <BoundaryPolicy Boundary>
		constexpr void RayCastEdge(const Vec2& a, const Vec2& b, const bool aAbove, const bool bAbove, const Vec2& point, bool& inside, bool& onBoundary) noexcept
		{
			const bool crossesRay = (aAbove != bAbove);
			const double cross = EdgeSide(a, b, point);

			inside ^= (crossesRay & ((cross > 0.0) == bAbove));

			if constexpr (Boundary != BoundaryPolicy::Unspecified)
			{
				const unsigned inYRange = (crossesRay | (a.y == point.y) | (b.y == point.y));
				const unsigned inXRange = (((a.x <= point.x) & (point.x <= b.x)) | ((b.x <= point.x) & (point.x <= a.x)));

				onBoundary |= ((cross == 0.0) & inYRange & inXRange);
			}
		}

		/// @brief ray casting(偶奇則)による点包含(3 頂点・巻き方向不問・縮退許容)
		/// @tparam Boundary 境界上の点の扱い
		/// @remark レイ交差の偶奇は巻き数の偶奇に等しく、縮退した(面積ゼロの)閉路の巻き数は
		///         任意の図形外の点で 0 になるため、縮退した入力でも「図形外 → false」が
		///         保証されます。境界上の点は `RayCastEdge` の境界検出で処理します。
		template <BoundaryPolicy Boundary>
		[[nodiscard]]
		constexpr bool ContainsPointRayCasting3(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& point) noexcept
		{
			const bool above0 = (p0.y > point.y);
			const bool above1 = (p1.y > point.y);
			const bool above2 = (p2.y > point.y);

			bool inside = false;
			bool onBoundary = false; // Unspecified では未使用のまま最適化で消える

			RayCastEdge<Boundary>(p0, p1, above0, above1, point, inside, onBoundary);
			RayCastEdge<Boundary>(p1, p2, above1, above2, point, inside, onBoundary);
			RayCastEdge<Boundary>(p2, p0, above2, above0, point, inside, onBoundary);

			if constexpr (Boundary == BoundaryPolicy::Unspecified)
			{
				return inside;
			}
			else
			{
				if (onBoundary)
				{
					return (Boundary == BoundaryPolicy::Included);
				}

				return inside;
			}
		}

		/// @brief ray casting(偶奇則)による点包含(4 頂点・凹可・巻き方向不問・縮退許容)
		/// @remark 3 頂点版 `ContainsPointRayCasting3` と同じ方式です。そちらの説明を参照。
		template <BoundaryPolicy Boundary>
		[[nodiscard]]
		constexpr bool ContainsPointRayCasting4(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& point) noexcept
		{
			const bool above0 = (p0.y > point.y);
			const bool above1 = (p1.y > point.y);
			const bool above2 = (p2.y > point.y);
			const bool above3 = (p3.y > point.y);

			bool inside = false;
			bool onBoundary = false; // Unspecified では未使用のまま最適化で消える

			RayCastEdge<Boundary>(p0, p1, above0, above1, point, inside, onBoundary);
			RayCastEdge<Boundary>(p1, p2, above1, above2, point, inside, onBoundary);
			RayCastEdge<Boundary>(p2, p3, above2, above3, point, inside, onBoundary);
			RayCastEdge<Boundary>(p3, p0, above3, above0, point, inside, onBoundary);

			if constexpr (Boundary == BoundaryPolicy::Unspecified)
			{
				return inside;
			}
			else
			{
				if (onBoundary)
				{
					return (Boundary == BoundaryPolicy::Included);
				}

				return inside;
			}
		}
	}

	namespace Geometry2D
	{
		template <PointContainmentOptions Options>
		bool ContainsPoint(const std::span<const Vec2> vertices, const Vec2& point) noexcept
		{
			const size_t n = vertices.size();

			if (n < 3)
			{
				return false;
			}

			const double px = point.x;
			const double py = point.y;
			const Vec2* const v = vertices.data();

			double ax = v[n - 1].x;
			double ay = v[n - 1].y;

			if constexpr ((Options.shape == PolygonShape::ConvexClockwise)
				|| (Options.shape == PolygonShape::ConvexCounterClockwise))
			{
				// 半平面判定:
				// Siv3D 座標系(x 右, y 下)の凸多角形は、
				//   CW  なら内部または境界上で全辺 cross >= 0
				//   CCW なら内部または境界上で全辺 cross <= 0
				// CCW の場合は cross を反転して CW と同じ判定に正規化する(コンパイル時に畳まれる)。
				//
				// 比較を `not (x >= 0)` / `not (x > 0)` の形にしているのは意図的:
				// 数学的には `x < 0` / `x <= 0` と同値だが、NaN が混入した場合に
				// 前者は false 側(= 早期 return で全体 false)に落ちるため、
				// 誤って true を返すことがない。コストは同一。
				constexpr bool IsClockwise = (Options.shape == PolygonShape::ConvexClockwise);

				for (size_t i = 0; i < n; ++i)
				{
					const double bx = v[i].x;
					const double by = v[i].y;

					const double cross = (bx - ax) * (py - ay) - (by - ay) * (px - ax);
					const double oriented = (IsClockwise ? cross : -cross);

					if constexpr (Options.boundary == BoundaryPolicy::Excluded)
					{
						if (not (oriented > 0.0))
						{
							return false;
						}
					}
					else
					{
						if (not (oriented >= 0.0))
						{
							return false;
						}
					}

					ax = bx;
					ay = by;
				}

				return true;
			}
			else if constexpr (Options.shape == PolygonShape::Convex)
			{
				// 凸だが巻き方向不明:
				// 全辺の cross の符号が一貫していれば内部または境界上。
				// (有界な凸多角形では、厳密に外部の点は必ず両符号を生じるため、
				//  cross == 0 を無視しても外部点が true になることはない)
				bool hasPositive = false;
				bool hasNegative = false;

				for (size_t i = 0; i < n; ++i)
				{
					const double bx = v[i].x;
					const double by = v[i].y;

					const double cross = (bx - ax) * (py - ay) - (by - ay) * (px - ax);

					if constexpr (Options.boundary == BoundaryPolicy::Excluded)
					{
						// 凸多角形の厳密な内部点は、どの辺の支持直線上にも乗らない。
						if (cross == 0.0)
						{
							return false;
						}
					}

					hasPositive |= (cross > 0.0);
					hasNegative |= (cross < 0.0);

					if (hasPositive && hasNegative)
					{
						return false;
					}

					ax = bx;
					ay = by;
				}

				return true;
			}
			else
			{
				// 一般の単純多角形:
				// ray casting(偶奇則)。巻き方向は結果に影響しない。
				bool inside = false;
				bool aAbove = (ay > py);

				for (size_t i = 0; i < n; ++i)
				{
					const double bx = v[i].x;
					const double by = v[i].y;
					const bool bAbove = (by > py);

					if constexpr (Options.boundary == BoundaryPolicy::Unspecified)
					{
						if (aAbove != bAbove)
						{
							const double cross = (bx - ax) * (py - ay) - (by - ay) * (px - ax);

							inside ^= ((cross > 0.0) == bAbove);
						}
					}
					else
					{
						// このゲートは「py が辺の閉 y 範囲 [min(ay, by), max(ay, by)] に含まれる」と等価。
						// 境界上判定・交差判定ともこの範囲外では成立しないため、
						// 大多数の辺で外積計算をスキップできる。
						if ((aAbove != bAbove) || (ay == py) || (by == py))
						{
							const double cross = (bx - ax) * (py - ay) - (by - ay) * (px - ax);

							// 境界上判定。y 範囲は上のゲートで確認済み。
							// x 範囲チェックは水平辺のために必要。
							if ((cross == 0.0)
								&& (((ax <= px) && (px <= bx)) || ((bx <= px) && (px <= ax))))
							{
								return (Options.boundary == BoundaryPolicy::Included);
							}

							// 交差判定(半開区間ルールにより頂点通過の二重カウントを防ぐ)
							if ((aAbove != bAbove) && ((cross > 0.0) == bAbove))
							{
								inside = (not inside);
							}
						}
					}

					ax = bx;
					ay = by;
					aAbove = bAbove;
				}

				return inside;
			}
		}

		/// @brief 点が三角形(3 頂点)の内部にあるかを返します。
		/// @tparam Options 点包含判定のコンパイル時設定
		/// @param p0 三角形の頂点 0
		/// @param p1 三角形の頂点 1
		/// @param p2 三角形の頂点 2
		/// @param point 判定する点
		/// @return 点が三角形の内部にある場合 true, それ以外の場合は false
		///
		/// @remark `BoundaryPolicy` の意味は多頂点版と同じです(Included: 境界上 true /
		///         Excluded: 境界上 false / Unspecified: 境界上の結果は未規定)。
		/// @remark 多頂点版と異なり、縮退した入力(頂点が同一直線上・重複・一致)に全設定で対応します。
		///         縮退した三角形は内部が空集合の図形として扱われます。すなわち `Included` では
		///         点が縮退図形(線分または点)上にある場合に true、`Excluded` では常に false、
		///         `Unspecified` では図形上の点の結果は未規定です。縮退・非縮退を問わず、厳密に
		///         外部の点(支持直線の延長上を含む)には false を返します。
		/// @remark 3 頂点は常に凸であるため、`PolygonShape` はどれを指定しても正しく動作します。
		///         既定値は `Convex` です(4 頂点版・多頂点版の既定 `General` とは異なります)。
		///         巻き方向が既知なら `ConvexClockwise` / `ConvexCounterClockwise` の指定で
		///         さらに高速になります。
		/// @remark 座標比較は厳密な `double` 比較に基づきます。浮動小数点誤差を含む境界上判定は保証されません。
		///
		/// @pre `Options.shape` が `ConvexClockwise` / `ConvexCounterClockwise` の場合、非縮退の入力は
		///      Siv3D 座標系(x 右, y 下)でそれぞれ時計回り / 反時計回りである必要があります
		///      (縮退した入力に巻き方向の要件はありません)。
		/// @pre `point` および各頂点の座標は有限値である必要があります。NaN / Inf はサポートされません。
		template <PointContainmentOptions Options>
		constexpr bool ContainsPoint(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& point) noexcept
		{
			if constexpr (Options.shape == PolygonShape::ConvexClockwise)
			{
				return detail::ContainsPointHalfPlane3<Options.boundary, true>(p0, p1, p2, point);
			}
			else if constexpr (Options.shape == PolygonShape::ConvexCounterClockwise)
			{
				return detail::ContainsPointHalfPlane3<Options.boundary, false>(p0, p1, p2, point);
			}
			else if constexpr (Options.shape == PolygonShape::Convex)
			{
				return detail::ContainsPointSignConsistency3<Options.boundary>(p0, p1, p2, point);
			}
			else
			{
				return detail::ContainsPointRayCasting3<Options.boundary>(p0, p1, p2, point);
			}
		}

		/// @brief 点が四角形(4 頂点)の内部にあるかを返します。
		/// @tparam Options 点包含判定のコンパイル時設定
		/// @param p0 四角形の頂点 0
		/// @param p1 四角形の頂点 1
		/// @param p2 四角形の頂点 2
		/// @param p3 四角形の頂点 3
		/// @param point 判定する点
		/// @return 点が四角形の内部にある場合 true, それ以外の場合は false
		///
		/// @remark `BoundaryPolicy` の意味は多頂点版と同じです(Included: 境界上 true /
		///         Excluded: 境界上 false / Unspecified: 境界上の結果は未規定)。
		/// @remark `PolygonShape::General` は凹四角形にも対応します。
		/// @remark 多頂点版と異なり、外周順を保つ縮退(隣接する頂点の重複によるゼロ長辺、平坦な頂点、
		///         全頂点の同一直線上・同一点への退化)に全設定で対応します。内部が空になる縮退図形は
		///         `Included` では図形(線分または点)上の点に true、`Excluded` では常に false を返し、
		///         `Unspecified` では図形上の点の結果は未規定です。隣接する頂点の重複により実質三角形と
		///         なる場合など、内部が空でない入力では通常どおり内部判定が行われます。縮退・非縮退を
		///         問わず、厳密に外部の点には false を返します。
		/// @remark 座標比較は厳密な `double` 比較に基づきます。浮動小数点誤差を含む境界上判定は保証されません。
		///
		/// @pre 自己交差する四角形、および頂点列が外周順を保たない入力(例: 辺を折り返す
		///      A, B, A, C のような順序)はサポートされません。
		/// @pre `Options.shape` が `Convex` / `ConvexClockwise` / `ConvexCounterClockwise` の場合、
		///      入力は凸四角形(ゼロ長辺・平坦な頂点を含んでよい)、または全頂点が同一直線上・
		///      同一点に退化した四角形である必要があります(凹四角形には `General` を指定)。
		/// @pre `Options.shape` が `ConvexClockwise` / `ConvexCounterClockwise` の場合、非縮退の入力は
		///      Siv3D 座標系(x 右, y 下)でそれぞれ時計回り / 反時計回りである必要があります
		///      (縮退した入力に巻き方向の要件はありません)。
		/// @pre `point` および各頂点の座標は有限値である必要があります。NaN / Inf はサポートされません。
		template <PointContainmentOptions Options>
		constexpr bool ContainsPoint(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& point) noexcept
		{
			if constexpr (Options.shape == PolygonShape::ConvexClockwise)
			{
				return detail::ContainsPointHalfPlane4<Options.boundary, true>(p0, p1, p2, p3, point);
			}
			else if constexpr (Options.shape == PolygonShape::ConvexCounterClockwise)
			{
				return detail::ContainsPointHalfPlane4<Options.boundary, false>(p0, p1, p2, p3, point);
			}
			else if constexpr (Options.shape == PolygonShape::Convex)
			{
				return detail::ContainsPointSignConsistency4<Options.boundary>(p0, p1, p2, p3, point);
			}
			else
			{
				return detail::ContainsPointRayCasting4<Options.boundary>(p0, p1, p2, p3, point);
			}
		}
	}
}

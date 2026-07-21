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
# include <span>
# include "../2DShapes.hpp"

namespace s3d
{
	/// @brief 境界上の点の扱い
	enum class BoundaryPolicy
	{
		/// @brief 境界上の点の結果を規定しない
		/// @remark 境界判定を省略できるため、最速の設定です。
		/// @remark 境界上の点に対して true / false のどちらが返るかは保証されません。
		Unspecified,

		/// @brief 境界上の点を内部として扱う
		/// @remark 点が辺または頂点上にある場合、true を返します。
		Included,

		/// @brief 境界上の点を外部として扱う
		/// @remark 点が辺または頂点上にある場合、false を返します。
		Excluded,
	};

	/// @brief 多角形の形状に関する前提
	/// @remark 凸性・巻き方向は最適化のための前提条件であり、実行時には検証されません。
	enum class PolygonShape
	{
		/// @brief 一般の単純多角形
		/// @remark 凹多角形を含みます。巻き方向は判定結果に影響しません。
		General,

		/// @brief 凸多角形(巻き方向不明)
		Convex,

		/// @brief 凸多角形、Siv3D 座標系(x 右, y 下)で時計回り
		ConvexClockwise,

		/// @brief 凸多角形、Siv3D 座標系(x 右, y 下)で反時計回り
		ConvexCounterClockwise,
	};

	/// @brief 点包含判定のコンパイル時設定
	/// @remark `shape` は最適化のための前提条件です。実際の入力と異なる値を指定した場合、結果は保証されません。
	struct PointContainmentOptions
	{
		/// @brief 境界上の点の扱い
		BoundaryPolicy boundary = BoundaryPolicy::Unspecified;

		/// @brief 多角形の形状に関する前提
		PolygonShape shape = PolygonShape::General;
	};

	namespace Geometry2D
	{
		/// @brief 点が多角形の内部にあるかを返します。
		/// @tparam Options 点包含判定のコンパイル時設定
		/// @tparam PointType 多角形の頂点の型
		/// @param vertices 多角形の外周頂点
		/// @param point 判定する点
		/// @return 点が多角形の内部にある場合 true, それ以外の場合は false
		///
		/// @remark `BoundaryPolicy::Included` の場合、境界上の点を true として扱います。
		/// @remark `BoundaryPolicy::Excluded` の場合、境界上の点を false として扱います。
		/// @remark `BoundaryPolicy::Unspecified` の場合、境界上の点の結果は保証されません。
		/// @remark `PolygonShape::ConvexClockwise` / `ConvexCounterClockwise` は最速の半平面判定を使用します。
		///         このパスでは `Unspecified` / `Included` は同一のコードになり、境界上の点は
		///         (厳密な `double` 比較が成立する範囲で)内部として扱われます。
		/// @remark `PolygonShape::Convex`(巻き方向不明)は外積符号の一貫性判定を使用します。
		/// @remark `PolygonShape::General` は非凸多角形にも対応する ray casting 判定を使用します。
		/// @remark `vertices.size() < 3` の場合は false を返します。
		/// @remark 座標比較は厳密な `double` 比較に基づきます。浮動小数点誤差を含む境界上判定は保証されません。
		///
		/// @pre `vertices` は外周順に並んだ、自己交差しない多角形の頂点列である必要があります。
		///      隣接する同一頂点(ゼロ長辺)を含む場合は、それらを取り除いた多角形が
		///      自己交差しない必要があります。
		/// @pre `vertices` は正の面積を持つ非縮退な多角形領域を表す必要があります。
		///      全頂点が同一直線上にある入力や、実質的に線分・点に退化した入力はサポートされません。
		/// @pre `Options.shape` が `Convex` / `ConvexClockwise` / `ConvexCounterClockwise` の場合、
		///      `vertices` は凸多角形である必要があります。
		/// @pre `Options.shape` が `ConvexClockwise` / `ConvexCounterClockwise` の場合、
		///      頂点列は Siv3D 座標系(x 右, y 下)でそれぞれ時計回り / 反時計回りである必要があります。
		/// @pre `Options.shape` が `Convex` / `ConvexClockwise` / `ConvexCounterClockwise`、かつ
		///      `Options.boundary == BoundaryPolicy::Excluded` の場合、隣接する同一頂点、すなわち
		///      ゼロ長辺を含んではいけません。それ以外の設定ではゼロ長辺は判定結果に影響しません。
		/// @pre `point` および `vertices` の各座標は有限値である必要があります。NaN / Inf はサポートされません。
		template <PointContainmentOptions Options = PointContainmentOptions{}, class PointType = Vec2>
		[[nodiscard]]
		bool ContainsPoint(std::span<const PointType> vertices, const Vec2& point) noexcept;

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
		template <PointContainmentOptions Options = PointContainmentOptions{ .boundary = BoundaryPolicy::Unspecified, .shape = PolygonShape::Convex }>
		[[nodiscard]]
		constexpr bool ContainsPoint(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& point) noexcept;

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
		template <PointContainmentOptions Options = PointContainmentOptions{}>
		[[nodiscard]]
		constexpr bool ContainsPoint(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, const Vec2& point) noexcept;
	}
}

#include "detail/PointContainment.ipp"

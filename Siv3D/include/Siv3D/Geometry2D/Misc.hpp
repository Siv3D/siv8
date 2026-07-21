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
# include "../Array.hpp"
# include "../Polygon.hpp"
# include "../MultiPolygon.hpp"

namespace s3d
{
	namespace Geometry2D
	{
		//////////////////////////////////////////////////
		//
		//	And
		//
		//////////////////////////////////////////////////

		/// @brief 長方形と多角形の共通部分を計算します。
		/// @param a 長方形
		/// @param b 多角形
		/// @return `a` と `b` の両方に含まれる領域からなる `MultiPolygon`。面積を持つ共通部分が存在しない場合は、空の `MultiPolygon`
		/// @remark 境界上の点や線分だけを共有する場合、その共通部分は結果に含まれません。
		[[nodiscard]]
		MultiPolygon And(const RectF& a, const Polygon& b);

		/// @brief 多角形と長方形の共通部分を計算します。
		/// @param a 多角形
		/// @param b 長方形
		/// @return `a` と `b` の両方に含まれる領域からなる `MultiPolygon`。面積を持つ共通部分が存在しない場合は、空の `MultiPolygon`
		/// @remark 境界上の点や線分だけを共有する場合、その共通部分は結果に含まれません。
		[[nodiscard]]
		MultiPolygon And(const Polygon& a, const RectF& b);

		/// @brief 2 つの多角形の共通部分を計算します。
		/// @param a 多角形
		/// @param b もう一方の多角形
		/// @return `a` と `b` の両方に含まれる領域からなる `MultiPolygon`。面積を持つ共通部分が存在しない場合は、空の `MultiPolygon`
		/// @remark 境界上の点や線分だけを共有する場合、その共通部分は結果に含まれません。
		[[nodiscard]]
		MultiPolygon And(const Polygon& a, const Polygon& b);

		//////////////////////////////////////////////////
		//
		//	Or
		//
		//////////////////////////////////////////////////

		/// @brief 長方形と多角形の和集合を計算します。
		/// @param a 長方形
		/// @param b 多角形
		/// @return `a` または `b` の少なくとも一方に含まれる領域からなる `MultiPolygon`
		[[nodiscard]]
		MultiPolygon Or(const RectF& a, const Polygon& b);

		/// @brief 多角形と長方形の和集合を計算します。
		/// @param a 多角形
		/// @param b 長方形
		/// @return `a` または `b` の少なくとも一方に含まれる領域からなる `MultiPolygon`
		[[nodiscard]]
		MultiPolygon Or(const Polygon& a, const RectF& b);

		/// @brief 2 つの多角形の和集合を計算します。
		/// @param a 多角形
		/// @param b もう一方の多角形
		/// @return `a` または `b` の少なくとも一方に含まれる領域からなる `MultiPolygon`
		[[nodiscard]]
		MultiPolygon Or(const Polygon& a, const Polygon& b);

		/// @brief 複数の多角形と 1 つの多角形の和集合を計算します。
		/// @param a 複数の多角形
		/// @param b 多角形
		/// @return `a` に含まれる領域と `b` に含まれる領域の和集合からなる `MultiPolygon`
		[[nodiscard]]
		MultiPolygon Or(const MultiPolygon& a, const Polygon& b);

		//////////////////////////////////////////////////
		//
		//	Xor
		//
		//////////////////////////////////////////////////

		/// @brief 長方形と多角形の対称差を計算します。
		/// @param a 長方形
		/// @param b 多角形
		/// @return `a` または `b` の一方だけに含まれる領域からなる `MultiPolygon`
		[[nodiscard]]
		MultiPolygon Xor(const RectF& a, const Polygon& b);

		/// @brief 多角形と長方形の対称差を計算します。
		/// @param a 多角形
		/// @param b 長方形
		/// @return `a` または `b` の一方だけに含まれる領域からなる `MultiPolygon`
		[[nodiscard]]
		MultiPolygon Xor(const Polygon& a, const RectF& b);

		/// @brief 2 つの多角形の対称差を計算します。
		/// @param a 多角形
		/// @param b もう一方の多角形
		/// @return `a` または `b` の一方だけに含まれる領域からなる `MultiPolygon`
		[[nodiscard]]
		MultiPolygon Xor(const Polygon& a, const Polygon& b);

		//////////////////////////////////////////////////
		//
		//	Subtract
		//
		//////////////////////////////////////////////////

		/// @brief 長方形から多角形に含まれる領域を取り除きます。
		/// @param a 元になる長方形
		/// @param b `a` から取り除く多角形
		/// @return `a` に含まれ、`b` には含まれない領域からなる `MultiPolygon`
		[[nodiscard]]
		MultiPolygon Subtract(const RectF& a, const Polygon& b);

		/// @brief 多角形から長方形に含まれる領域を取り除きます。
		/// @param a 元になる多角形
		/// @param b `a` から取り除く長方形
		/// @return `a` に含まれ、`b` には含まれない領域からなる `MultiPolygon`
		[[nodiscard]]
		MultiPolygon Subtract(const Polygon& a, const RectF& b);

		/// @brief 多角形から別の多角形に含まれる領域を取り除きます。
		/// @param a 元になる多角形
		/// @param b `a` から取り除く多角形
		/// @return `a` に含まれ、`b` には含まれない領域からなる `MultiPolygon`
		[[nodiscard]]
		MultiPolygon Subtract(const Polygon& a, const Polygon& b);

		//////////////////////////////////////////////////
		//
		//	DiscreteFrechetDistance
		//
		//////////////////////////////////////////////////

		/// @brief 2 つの折れ線間の離散 Fréchet 距離を計算します。
		/// @param a 折れ線
		/// @param b もう一方の折れ線
		/// @return `a` と `b` の離散 Fréchet 距離。どちらか一方が空の場合は正の無限大
		/// @remark 各折れ線の頂点を順番どおりに対応付けたときの距離です。線分内部の点は評価対象に含まれません。
		/// @remark 折れ線の進行方向および頂点の順序が結果に影響します。
		[[nodiscard]]
		double DiscreteFrechetDistance(const LineString& a, const LineString& b);

		//////////////////////////////////////////////////
		//
		//	DiscreteHausdorffDistance
		//
		//////////////////////////////////////////////////

		/// @brief 2 つの折れ線間の離散 Hausdorff 距離を計算します。
		/// @param a 折れ線
		/// @param b もう一方の折れ線
		/// @return `a` と `b` の離散 Hausdorff 距離。どちらか一方が空の場合は正の無限大
		/// @remark 各折れ線の頂点集合を比較します。線分内部の点および頂点の通過順序は評価対象に含まれません。
		[[nodiscard]]
		double DiscreteHausdorffDistance(const LineString& a, const LineString& b);

		//////////////////////////////////////////////////
		//
		//	ComposePolygons
		//
		//////////////////////////////////////////////////

		/// @brief 複数のリングから多角形を組み立てます。
		/// @param rings 多角形の境界として使用するリングの配列。外周リングは時計回り、穴を表すリングは反時計回り
		/// @return 入力リングから組み立てられた多角形群。多角形を構成できない場合は、空の `MultiPolygon`
		/// @remark 各リングは自己交差せず、互いの境界が不正に交差しない有効なリングである必要があります。
		/// @remark 穴を表すリングは、対応する外周リングの内部に配置する必要があります。
		[[nodiscard]]
		MultiPolygon ComposePolygons(const Array<LineString>& rings);
	}
}

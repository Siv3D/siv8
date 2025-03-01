//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <span>
# include <Siv3D/Array.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/TriangleIndex.hpp>
# include "GeometryCommon.hpp"

namespace s3d
{
	struct ColorF;
	struct PatternParameters;

	/// @brief 外周頂点と穴の頂点をもとに多角形を三角形分割します。
	/// @param outer 外周の頂点
	/// @param holes 穴の頂点
	/// @param dstIndices 生成された三角形のインデックス
	/// @return 三角形分割に成功した場合 true, それ以外の場合は false
	[[nodiscard]]
	bool Triangulate(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes, Array<TriangleIndex>& dstIndices);

	/// @brief 外周頂点と穴の頂点をもとに多角形を三角形分割し、描画します。
	/// @param outer 外周の頂点
	/// @param holes 穴の頂点
	/// @param color 多角形の色
	/// @remark Polygon の作成を経由せずに、三角形分割の結果を描画で使用する関数です。
	void DrawTriangles(std::span<const Vec2> outer, const InnersType& holes, const ColorF& color);

	/// @brief 外周頂点と穴の頂点をもとに多角形を三角形分割し、描画します。
	/// @param outer 外周の頂点
	/// @param holes 穴の頂点
	/// @param pattern 塗りつぶしのパターン
	/// @remark Polygon の作成を経由せずに、三角形分割の結果を描画で使用する関数です。
	void DrawTriangles(std::span<const Vec2> outer, const InnersType& holes, const PatternParameters& pattern);
}

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
# include <Siv3D/Array.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/TriangleIndex.hpp>

namespace s3d
{
	/// @brief 外周頂点と穴の頂点をもとに多角形を三角形分割します。
	/// @param outer 外周の頂点
	/// @param holes 穴の頂点
	/// @param dstIndices 生成された三角形のインデックス
	/// @return 1 個以上の三角形を生成できた場合 true, それ以外の場合は false
	[[nodiscard]]
	bool Triangulate(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes, Array<TriangleIndex>& dstIndices);

}

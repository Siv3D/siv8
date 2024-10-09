//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
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
	[[nodiscard]]
	bool Triangulate(std::span<const Vec2> outer, const Array<Array<Vec2>>& holes, Array<TriangleIndex>& dstIndices);
}

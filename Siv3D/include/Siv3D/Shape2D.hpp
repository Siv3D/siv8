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
# include "Common.hpp"
# include "Array.hpp"
# include "PointVector.hpp"
# include "TriangleIndex.hpp"
# include "2DShapes.hpp"

namespace s3d
{
	/// @brief 多角形作成ユーティリティ
	class Shape2D
	{
	public:


	private:

		Array<Float2> m_vertices;

		Array<TriangleIndex> m_indices;
	};
}

# include "detail/Shape2D.ipp"

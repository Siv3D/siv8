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
# include "Types.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	GridConnectivity
	//
	////////////////////////////////////////////////////////////////

	/// @brief グリッド上のセルの連結方法 | Cell connectivity on a grid
	enum class GridConnectivity : uint8
	{
		/// @brief 上下左右の 4 方向で連結します。
		Four,

		/// @brief 上下左右と斜めの 8 方向で連結します。
		Eight,
	};
}

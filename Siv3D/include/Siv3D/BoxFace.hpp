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
# include "Common.hpp"
# include <ThirdParty/EnumBitmask/EnumBitmask.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	BoxFace
	//
	////////////////////////////////////////////////////////////////

	/// @brief 直方体で生成する面を表すビットマスク
	enum class BoxFace : uint8
	{
		/// @brief 面を生成しない
		None_		= 0,

		/// @brief 法線が X 軸の負方向を向く面
		NegativeX	= (1 << 0),

		/// @brief 法線が X 軸の正方向を向く面
		PositiveX	= (1 << 1),

		/// @brief 法線が Y 軸の負方向を向く面
		NegativeY	= (1 << 2),

		/// @brief 法線が Y 軸の正方向を向く面
		PositiveY	= (1 << 3),

		/// @brief 法線が Z 軸の負方向を向く面
		NegativeZ	= (1 << 4),

		/// @brief 法線が Z 軸の正方向を向く面
		PositiveZ	= (1 << 5),

		/// @brief すべての面
		All			= (NegativeX | PositiveX | NegativeY | PositiveY | NegativeZ | PositiveZ),
	};
	DEFINE_BITMASK_OPERATORS(BoxFace);
}

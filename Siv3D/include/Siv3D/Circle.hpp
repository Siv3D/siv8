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
# include "PointVector.hpp"
# include "ColorHSV.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	/// @brief 円
	struct Circle
	{
		using position_type	= Vec2;

		using size_type		= position_type::value_type;

		using value_type	= position_type::value_type;

	SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4201)

		union
		{
			/// @brief 円の中心座標
			position_type center;

			struct
			{
				/// @brief 円の中心の X 座標
				value_type x;

				/// @brief 円の中心の Y 座標
				value_type y;
			};
		};

		/// @brief 円の半径
		/// @remark この値が負の時の挙動は未規定です。
		size_type r;

	SIV3D_DISABLE_MSVC_WARNINGS_POP()
		
	};
}

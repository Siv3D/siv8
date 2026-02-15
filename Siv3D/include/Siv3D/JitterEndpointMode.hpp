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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	JitterEndpointMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief 端点の揺れ制御モード | Jitter endpoint behavior
	enum class JitterEndpointMode : uint8
	{
		/// @brief 制御なし（通常どおり揺らす） | No endpoint constraint; jitter is fully applied
		Free,

		/// @brief 端点に向かって揺れを徐々に減衰させる | Gradually attenuate jitter toward the endpoint
		Tapered,

		/// @brief 端点を元の位置に固定する | Lock the endpoint to its original position
		Locked
	};
}

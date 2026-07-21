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
# include "../Common.hpp"
# include "JitterEndpointMode.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	JitterEndpointSettings
	//
	////////////////////////////////////////////////////////////////

	/// @brief 端点の揺れ制御設定 | Jitter endpoint settings
	struct JitterEndpointSettings
	{
		/// @brief 端点の揺れ制御モード | Endpoint behavior mode
		JitterEndpointMode mode = JitterEndpointMode::Free;

		/// @brief 減衰の範囲（0.0〜0.5） | Taper length ratio (0.0 to 0.5)
		/// @remark mode == JitterEndpointMode::Tapered の場合にのみ参照されます。 | Used only when mode == JitterEndpointMode::Tapered.
		double taperRatio = 0.0;

		[[nodiscard]]
		static constexpr JitterEndpointSettings Free() noexcept;

		[[nodiscard]]
		static constexpr JitterEndpointSettings Tapered(double taperRatio = 0.25) noexcept;

		[[nodiscard]]
		static constexpr JitterEndpointSettings Locked() noexcept;
	};
}

# include "detail/JitterEndpointSettings.ipp"

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
	/// @brief 外部電源の接続状態
	enum class ExternalPowerStatus : uint8
	{
		/// @brief 不明
		Unknown,

		/// @brief 接続されていない（バッテリー駆動）
		NotConnected,

		/// @brief 接続されている
		Connected,
	};
}

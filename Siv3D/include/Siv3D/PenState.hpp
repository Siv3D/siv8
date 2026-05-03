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

namespace s3d
{
	/// @brief ペンタブレットの入力状態
	struct PenState
	{
		/// @brief ペンがタブレットの検出範囲内（近接中）であるか
		bool inProximity = false;

		/// @brief ペンモードであるか
		bool isPen = true;

		/// @brief 消しゴムモードであるか
		bool isEraser = false;

		/// @brief 法線方向（ペン先）の筆圧（0.0～1.0）
		double normalPressure = 0.0;

		/// @brief 接線方向（バレル／タンジェント）の筆圧（0.0～1.0）
		/// @remark 一般的なペンでは未対応です（例：エアブラシ等の特殊入力で提供されることがあります）
		double tangentialPressure = 0.0;

		/// @brief 方位角（0.0～360.0）
		double azimuth = 0.0;

		/// @brief 仰角（-90.0～90.0）
		double altitude = 0.0;

		/// @brief ねじれ（0.0～360.0）
		double twist = 0.0;

		/// @brief ピッチ
		double pitch = 0.0;

		/// @brief ロール
		double roll = 0.0;

		/// @brief ヨー
		double yaw = 0.0;
	};
}

﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"

namespace s3d
{
	struct ProfilerStat;

	namespace Profiler
	{
		////////////////////////////////////////////////////////////////
		//
		//	FPS
		//
		////////////////////////////////////////////////////////////////

		/// @brief 1 秒間に更新されたフレーム数（フレームレート）を返します。
		/// @remark この値は 1 秒ごとに更新されます。
		/// @return 1 秒間に更新されたフレーム数
		[[nodiscard]]
		int32 FPS() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	EnableAssetCreationWarning
		//
		////////////////////////////////////////////////////////////////

		/// @brief アセットを毎フレーム連続して作成した場合の警告の ON / OFF を設定します。
		/// @param enabled 警告を有効にするか
		void EnableAssetCreationWarning(bool enabled);

		////////////////////////////////////////////////////////////////
		//
		//	GetStat
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const ProfilerStat& GetStat();
	}
}

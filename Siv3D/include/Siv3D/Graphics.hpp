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

namespace s3d
{
	namespace Graphics
	{
		/// @brief VSync の有効無効を設定します（デフォルトでは有効）。
		/// @param enabled VSync を有効にする場合 true, 無効にする場合 false
		void SetVSyncEnabled(bool enabled);

		/// @brief VSync が有効であるかを返します。
		/// @return VSync が有効である場合 true, 無効である場合 false
		[[nodiscard]]
		bool IsVSyncEnabled();

		/// @brief 低遅延モードに設定します（デフォルトでは無効）。
		/// @param enabled 低遅延モードを有効にする場合 true, 無効にする場合 false
		/// @remark 低遅延モードは入力遅延を最小限に抑えますが、高負荷時のフレームレート安定性が損なわれる場合があります。
		/// @remark Windows 版のみ有効です。
		void SetLowLatencyMode(bool enabled);

		/// @brief 低遅延モードが有効であるかを返します。
		/// @return 低遅延モードが有効である場合 true, 無効である場合 false
		[[nodiscard]]
		bool IsLowLatencyMode();
	}
}

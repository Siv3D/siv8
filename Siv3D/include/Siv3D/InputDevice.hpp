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
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	InputDevice
	//
	////////////////////////////////////////////////////////////////

	/// @brief 入力デバイスの種類
	enum class InputDevice : uint8
	{
		/// @brief 未定義
		Unknown,

		/// @brief キーボード
		Keyboard,

		/// @brief マウス
		Mouse,

		/// @brief ゲームパッド
		Gamepad,

		/// @brief XInput 対応ゲームコントローラー
		XInput,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, InputDevice value);
}

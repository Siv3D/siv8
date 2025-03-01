//-----------------------------------------------
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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	InputDeviceType
	//
	////////////////////////////////////////////////////////////////

	/// @brief 入力デバイスの種類
	enum class InputDeviceType : uint8
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

	/// @brief 入力デバイスの種類を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 入力デバイスの種類
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, InputDeviceType value);
}

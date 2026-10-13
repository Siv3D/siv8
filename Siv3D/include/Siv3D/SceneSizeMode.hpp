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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	SceneSizeMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief シーンサイズの決め方
	enum class SceneSizeMode : uint8
	{
		/// @brief 指定サイズに固定する
		Fixed,

		/// @brief DPI 倍率を除いたウィンドウの描画領域のサイズに合わせる
		Window,

		/// @brief ウィンドウの描画領域の実ピクセル数に合わせる
		WindowPixels,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief シーンサイズの方針を文字列に変換します。
	/// @param formatData 出力先
	/// @param value 変換する値
	void Formatter(FormatData& formatData, SceneSizeMode value);
}

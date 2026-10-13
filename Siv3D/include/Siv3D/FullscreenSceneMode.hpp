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
	//	FullscreenSceneMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief 全画面中のシーンサイズの扱い
	enum class FullscreenSceneMode : uint8
	{
		/// @brief シーンサイズの自動変更を止める
		KeepSize,

		/// @brief シーンサイズの方針に従って変更する
		FollowWindow,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 全画面中のシーンサイズの方針を文字列に変換します。
	/// @param formatData 出力先
	/// @param value 変換する値
	void Formatter(FormatData& formatData, FullscreenSceneMode value);
}

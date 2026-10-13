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
	//	SceneResolution
	//
	////////////////////////////////////////////////////////////////

	/// @brief シーンの描画解像度の決め方
	enum class SceneResolution : uint8
	{
		/// @brief シーンサイズと同じピクセル数で描く
		SceneSize,

		/// @brief シーンの表示領域に合わせて描画解像度を増やす。シーンサイズ未満には下げない
		Display,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 描画解像度の方針を文字列に変換します。
	/// @param formatData 出力先
	/// @param value 変換する値
	void Formatter(FormatData& formatData, SceneResolution value);
}

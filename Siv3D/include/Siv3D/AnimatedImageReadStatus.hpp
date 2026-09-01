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
	struct FormatData;

	/// @brief アニメーション画像のストリーム読み込み状態
	enum class AnimatedImageReadStatus : uint8
	{
		/// @brief フレームを読み込んだ
		Frame,

		/// @brief ストリームの終端に達した
		EndOfStream,

		/// @brief エラーが発生した
		Error,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief アニメーション画像のストリーム読み込み状態を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value アニメーション画像のストリーム読み込み状態
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, AnimatedImageReadStatus value);
}

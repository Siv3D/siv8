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

	/// @brief アニメーション画像のデコードエラー
	enum class AnimatedImageDecodeError : uint8
	{
		/// @brief エラーなし
		None,

		/// @brief 対応していない画像形式
		InvalidFormat,

		/// @brief PNG だが APNG ではない
		NotAnimated,

		/// @brief 画像データが不正
		InvalidData,

		/// @brief 画像データが途中で終わっている
		TruncatedInput,

		/// @brief Reader からの読み込みに失敗
		ReadError,

		/// @brief 未対応の画像機能を使用している
		UnsupportedFeature,

		/// @brief フレーム数の上限を超えた
		TooManyFrames,

		/// @brief デコード済み画像のバイト数上限を超えた
		DecodedBytesLimitExceeded,

		/// @brief 作業メモリ上限を超えた
		WorkingMemoryLimitExceeded,

		/// @brief メモリ確保に失敗
		OutOfMemory,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief アニメーション画像のデコードエラーを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value アニメーション画像のデコードエラー
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, AnimatedImageDecodeError value);
}

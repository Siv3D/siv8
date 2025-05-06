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

	/// @brief フォントの描画方式
	enum class FontMethod : uint8
	{
		/// @brief ビットマップ（デフォルト）
		/// @remark 利点: 文字キャッシュの生成が高速。正確な字形をレンダリングできる
		/// @remark 欠点: 基本サイズより拡大したときにぼやける。影、輪郭などの距離エフェクトを適用できない
		Bitmap,

		/// @brief MSDF
		/// @remark 利点: 基本サイズより拡大したときにぼやけない。影、輪郭などの距離エフェクトを適用できる
		/// @remark 欠点: 文字キャッシュの生成が遅い。基本サイズが足りないと複雑な文字にノイズがのる
		MSDF,
	};

	/// @brief フォントの描画方式を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value フォントの描画方式
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, FontMethod value);
}

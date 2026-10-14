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
	//	JoinStyle
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線の結合方法 | Line join style
	enum class JoinStyle : uint8
	{
		/// @brief 標準の結合方法を使用します。
		Default,

		/// @brief 内角が 60° 未満の角を、頂点から線幅の半分の距離にある、角の二等分線に垂直な直線で切り落とします。それ以外の角は Miter と同じです。
		Bevel,

		/// @brief 角を円弧で結合します。
		Round,

		/// @brief 辺の延長線の交点まで角を伸ばします。突出の長さに上限はありません。
		Miter,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線の結合方法を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 線の結合方法
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, JoinStyle value);
}

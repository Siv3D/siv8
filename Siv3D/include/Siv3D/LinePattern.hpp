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

	////////////////////////////////////////////////////////////////
	//
	//	LinePattern
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線のパターン | Line pattern
	enum class LinePattern : uint8
	{
		/// @brief 実線
		Solid,

		/// @brief ドット
		Dotted,

		/// @brief ダッシュ
		Dashed,

		/// @brief 長いダッシュ
		LongDash,

		/// @brief ダッシュとドット
		DashDot,

		/// @brief 丸いドット
		RoundDot,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 線のパターンを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 線のパターン
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, LinePattern value);
}

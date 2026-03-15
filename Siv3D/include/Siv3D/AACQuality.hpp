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
	//	AACQuality
	//
	////////////////////////////////////////////////////////////////

	/// @brief AAC エンコードの品質 | AAC encoding quality
	enum class AACQuality : uint8
	{
		/// @brief 低品質 | Low quality
		Low,

		/// @brief 中品質 | Medium quality
		Medium,

		/// @brief 高品質 | High quality
		High,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief AAC エンコードの品質を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value AAC エンコードの品質
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, AACQuality value);
}

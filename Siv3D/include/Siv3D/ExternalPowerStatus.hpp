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
	//	ExternalPowerStatus
	//
	////////////////////////////////////////////////////////////////

	/// @brief 外部電源の接続状態
	enum class ExternalPowerStatus : uint8
	{
		/// @brief 不明
		Unknown,

		/// @brief 接続されていない（バッテリー駆動）
		NotConnected,

		/// @brief 接続されている
		Connected,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 外部電源の接続状態を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 外部電源の接続状態
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, ExternalPowerStatus value);
}

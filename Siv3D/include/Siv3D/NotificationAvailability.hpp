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
	//	NotificationAvailability
	//
	////////////////////////////////////////////////////////////////

	/// @brief 通知機能の利用可否
	enum class NotificationAvailability : uint8
	{
		/// @brief 通知機能を利用できません
		Unavailable,

		/// @brief 通知機能の利用可否がまだ不明です
		/// @remark 権限確認や許可要求の結果がまだ反映されていない場合を含みます
		NotDetermined,

		/// @brief 通知機能の利用が拒否されています
		Denied,

		/// @brief 通知機能を利用できます
		Available,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief NotificationAvailability を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value NotificationAvailability
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, NotificationAvailability value);
}

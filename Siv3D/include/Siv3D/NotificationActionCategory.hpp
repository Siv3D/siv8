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

	/// @brief 通知に設定する定義済みアクションカテゴリ
	enum class NotificationActionCategory : uint8
	{
		/// @brief アクション無し
		Default = 0,
		
		/// @brief 「Open」
		/// @remark actionID は U"open" です
		Open,
		
		/// @brief 「Open」「Dismiss」
		/// @remark actionID はそれぞれ U"open", U"dismiss" です
		OpenDismiss,
		
		/// @brief 「OK」
		/// @remark actionID は U"ok" です
		OK,
		
		/// @brief 「OK」「Cancel」
		/// @remark actionID はそれぞれ U"ok", U"cancel" です
		OKCancel,
		
		/// @brief 「Yes」「No」
		/// @remark actionID はそれぞれ U"yes", U"no" です
		YesNo,
		
		/// @brief 「Yes」「No」「Cancel」
		/// @remark actionID はそれぞれ U"yes", U"no", U"cancel" です
		YesNoCancel,
		
		/// @brief 「Accept」「Decline」
		/// @remark actionID はそれぞれ U"accept", U"decline" です
		AcceptDecline,
		
		/// @brief 「Accept」「Decline」「Later」
		/// @remark actionID はそれぞれ U"accept", U"decline", U"later" です
		AcceptDeclineLater,
		
		/// @brief 「Retry」「Cancel」
		/// @remark actionID はそれぞれ U"retry", U"cancel" です
		RetryCancel,
		
		/// @brief 「View」「Dismiss」
		/// @remark actionID はそれぞれ U"view", U"dismiss" です
		ViewDismiss,
	};

	/// @brief NotificationActionCategory を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value NotificationActionCategory
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, NotificationActionCategory value);
}

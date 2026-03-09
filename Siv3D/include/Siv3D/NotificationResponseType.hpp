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

	/// @brief 通知に対するユーザまたはシステムからの応答の種類
	enum class NotificationResponseType : uint8
	{
		/// @brief 応答の種類が未指定です
		/// @remark 主にデフォルト初期値として使用されます
		Unspecified,

		/// @brief 通知本体の既定アクションが実行されました
		DefaultActivated,

		/// @brief 通知のアクションが実行されました
		ActionActivated,

		/// @brief 通知が閉じられました
		/// @remark 発生条件や取得可否はプラットフォームによって異なります
		Dismissed,

		/// @brief 通知の表示または処理に失敗しました
		Failed,
	};

	/// @brief NotificationResponseType を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value NotificationAvailability
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, NotificationResponseType value);
}

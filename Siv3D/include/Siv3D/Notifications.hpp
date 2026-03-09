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
# include "NotificationAvailability.hpp"
# include "NotificationRequest.hpp"
# include "NotificationResponse.hpp"

namespace s3d
{
	namespace Notifications
	{
		/// @brief 通知機能の現在把握している利用可否を返します
		/// @return 通知機能の利用可否
		/// @remark プラットフォームによっては利用可否の取得や更新が非同期で行われるため、直前の要求結果がまだ反映されていない場合があります
		[[nodiscard]]
		NotificationAvailability GetAvailability();

		/// @brief 通知の利用許可を要求します
		[[nodiscard]]
		void RequestPermission();

		/// @brief 通知を表示します
		/// @param request 表示する通知の内容
		/// @return 通知の表示に成功した場合はその通知 ID, 失敗した場合は none
		[[nodiscard]]
		Optional<NotificationID> Show(const NotificationRequest& request);

		/// @brief 指定した通知を閉じます
		/// @param id 通知 ID
		/// @remark すでに閉じられている通知や存在しない通知を指定した場合は何も行いません
		void Dismiss(NotificationID id);

		/// @brief すべての通知を閉じます
		void DismissAll();

		/// @brief 発生した通知応答を取得します
		/// @return 取得した通知応答の配列
		/// @remark この関数は内部に蓄積された応答を取り出して返します
		[[nodiscard]]
		Array<NotificationResponse> ExtractResponses();
	}
}

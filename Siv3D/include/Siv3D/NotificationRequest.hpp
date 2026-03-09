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
# include "String.hpp"
# include "Array.hpp"
# include "NotificationAction.hpp"

namespace s3d
{
	/// @brief 通知の内容
	struct NotificationRequest
	{
		/// @brief 通知のタイトル
		String title;

		/// @brief 通知の本文
		String body;

		/// @brief 通知に添付する画像のファイルパス
		/// @remark 対応状況や表示方法はプラットフォームによって異なります
		FilePath imagePath;

		/// @brief 通知に表示するアクション
		/// @remark アクションの対応数や表示方法はプラットフォームによって異なります
		Array<NotificationAction> actions;

		/// @brief 通知音を再生するか
		/// @remark 対応状況はプラットフォームやユーザ設定によって異なります
		bool playSound = true;
	};
}

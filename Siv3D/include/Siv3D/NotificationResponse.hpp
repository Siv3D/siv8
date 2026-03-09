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
# include "NotificationResponseType.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	NotificationID
	//
	////////////////////////////////////////////////////////////////

	/// @brief 通知 ID
	using NotificationID = uint64;

	////////////////////////////////////////////////////////////////
	//
	//	NotificationResponse
	//
	////////////////////////////////////////////////////////////////

	/// @brief 通知に対する応答
	struct NotificationResponse
	{
		/// @brief 応答元の通知 ID
		NotificationID id = 0;

		/// @brief 応答の種類
		NotificationResponseType type = NotificationResponseType::Unspecified;

		/// @brief 実行されたアクションの識別子
		/// @remark type が NotificationResponseType::ActionActivated の場合に設定されます
		String actionID = {};
	};
}

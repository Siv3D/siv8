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

namespace s3d
{
	/// @brief 通知アクション
	struct NotificationAction
	{
		/// @brief アクションの識別子
		/// @remark ユーザには表示されない、アプリケーション内部向けの安定した ID です
		String id;

		/// @brief アクションの表示名
		String label;
	};
}

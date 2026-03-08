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
	using NotificationID = int64;

	struct NotificationResponse
	{
		NotificationID id = 0;
		
		NotificationResponseType type = NotificationResponseType::Failed;
		
		String actionID;
	};
}

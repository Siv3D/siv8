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
	struct NotificationRequest
	{
		String title;

		String body;
		
		FilePath imagePath;
		
		Array<NotificationAction> actions;
		
		bool playSound = true;
	};
}

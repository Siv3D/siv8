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
		[[nodiscard]]
		NotificationAvailability GetAvailability();

		[[nodiscard]]
		bool RequestPermission();

		[[nodiscard]]
		Optional<NotificationID> Show(const NotificationRequest& request);

		void Dismiss(NotificationID id);

		void DismissAll();

		[[nodiscard]]
		Array<NotificationResponse> DrainResponses();
	}
}

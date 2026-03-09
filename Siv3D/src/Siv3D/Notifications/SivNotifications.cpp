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

# include <Siv3D/Notifications.hpp>
# include <Siv3D/Notifications/CNotifications.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Notifications
	{
		NotificationAvailability GetAvailability()
		{
			return SIV3D_ENGINE(Notifications)->getAvailability();
		}

		bool RequestPermission()
		{
			return SIV3D_ENGINE(Notifications)->requestPermission();
		}

		Optional<NotificationID> Show(const NotificationRequest& request)
		{
			return SIV3D_ENGINE(Notifications)->show(request);
		}

		void Dismiss(const NotificationID id)
		{
			return SIV3D_ENGINE(Notifications)->dismiss(id);
		}

		void DismissAll()
		{
			return SIV3D_ENGINE(Notifications)->dismissAll();
		}

		Array<NotificationResponse> TakeResponses()
		{
			return SIV3D_ENGINE(Notifications)->takeResponses();
		}
	}
}

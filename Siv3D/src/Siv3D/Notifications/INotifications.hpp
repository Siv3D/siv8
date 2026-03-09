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
# include <Siv3D/Common.hpp>
# include <Siv3D/Optional.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/NotificationAvailability.hpp>
# include <Siv3D/NotificationRequest.hpp>
# include <Siv3D/NotificationResponse.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DNotifications
	{
	public:

		[[nodiscard]]
		static ISiv3DNotifications* Create();

		virtual ~ISiv3DNotifications() = default;

		virtual void init() = 0;

		[[nodiscard]]
		virtual NotificationAvailability getAvailability() = 0;

		virtual void requestPermission() = 0;

		[[nodiscard]]
		virtual Optional<NotificationID> show(const NotificationRequest& request) = 0;

		virtual void dismiss(NotificationID id) = 0;

		virtual void dismissAll() = 0;

		[[nodiscard]]
		virtual Array<NotificationResponse> extractResponses() = 0;
	};
}

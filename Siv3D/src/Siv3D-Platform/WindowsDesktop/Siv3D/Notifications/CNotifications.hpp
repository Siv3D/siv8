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
# include <mutex>
# include <Siv3D/HashMap.hpp>
# include <Siv3D/Notifications/INotifications.hpp>

namespace s3d
{
	class CNotifications final : public ISiv3DNotifications
	{
	public:

		CNotifications();

		~CNotifications() override;

		NotificationAvailability getAvailability() override;

		bool requestPermission() override;

		Optional<NotificationID> show(const NotificationRequest& request) override;

		void dismiss(NotificationID id) override;

		void dismissAll() override;

		Array<NotificationResponse> extractResponses() override;

		void enqueueResponse(NotificationID id, NotificationResponseType responseType);

		void enqueueResponse(NotificationID id, NotificationResponseType responseType, const String& actionID);


	private:

		struct Entry
		{
			int64 platformID = -1;
		};

		Optional<NotificationAvailability> m_availability;

		////////////////////////////////////////////////////////////////
		//
		std::mutex m_mutex;

		HashMap<NotificationID, Entry> m_entries;

		Array<NotificationResponse> m_responseQueue;

		std::atomic<NotificationID> m_nextID{ 1 };
		//
		////////////////////////////////////////////////////////////////
	};
}

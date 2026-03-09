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
# include <atomic>
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
		
		void init() override;

		NotificationAvailability getAvailability() override;

		void requestPermission() override;

		Optional<NotificationID> show(const NotificationRequest& request) override;

		void dismiss(NotificationID id) override;

		void dismissAll() override;

		Array<NotificationResponse> extractResponses() override;

		void enqueueResponse(NotificationID id, NotificationResponseType responseType);

		void enqueueResponse(NotificationID id, NotificationResponseType responseType, const String& actionID);

		void updateAvailability(NotificationAvailability availability);

		Optional<NotificationID> findIDFromRequestIdentifier(const String& requestIdentifier);

	private:

		struct Entry
		{
			String requestIdentifier;
			String categoryIdentifier;
			Array<String> actionIDs;
		};

		void refreshAvailabilityAsync();

		Optional<NotificationAvailability> m_availability;

		////////////////////////////////////////////////////////////////
		//
		std::mutex m_mutex;

		HashMap<NotificationID, Entry> m_entries;

		HashMap<String, NotificationID> m_requestIdentifierToID;

		Array<NotificationResponse> m_responseQueue;

		std::atomic<NotificationID> m_nextID{ 1 };

		const void* m_delegate = nullptr; // Objective-C オブジェクト
		//
		////////////////////////////////////////////////////////////////
	};
}

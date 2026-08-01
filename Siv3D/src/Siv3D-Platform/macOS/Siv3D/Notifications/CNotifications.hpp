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
# include <memory>
# include <Siv3D/Notifications/INotifications.hpp>

namespace s3d
{
	struct NotificationsState;

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

	private:

		void refreshAvailabilityAsync();

		////////////////////////////////////////////////////////////////
		//
		// 非同期コールバックと共有し、CNotifications の破棄後も必要な状態を保持する
		std::shared_ptr<NotificationsState> m_state;

		std::atomic<NotificationID> m_nextID{ 1 };

		const void* m_delegate = nullptr; // Objective-C オブジェクト
		//
		////////////////////////////////////////////////////////////////
	};
}

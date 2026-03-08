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
# include <ThirdParty/WinToast/wintoastlib.h>

namespace s3d
{
	class CNotifications final : public ISiv3DNotifications
	{
	public:

		struct Notification
		{
			String actionID;

			/// @brief トースト通知の状態
			enum class State : uint8
			{
				/// @brief 無効
				Invalid,

				/// @brief 表示されている
				Shown,

				/// @brief ユーザによってアクティベートされた
				Activated,

				/// @brief ユーザによってキャンセルされた
				UserCanceled,

				/// @brief アプリケーションによって非表示にされた
				ApplicationHidden,

				/// @brief タイムアウトした
				TimedOut,

				/// @brief エラーが発生した
				Error,

			} state = State::Invalid;
		};

		~CNotifications() override;

		NotificationAvailability getAvailability() override;

		bool requestPermission() override;

		Optional<NotificationID> show(const NotificationRequest& request) override;

		void dismiss(NotificationID id) override;

		void dismissAll() override;

		Array<NotificationResponse> drainResponses() override;

		void onStateUpdate(NotificationID id, Notification::State state);

		void onStateUpdate(NotificationID id, Notification::State state, const String& actionID);

	private:

		Optional<NotificationAvailability> m_availability;

		////////////////////////////////////////////////////////////////
		//
		std::mutex m_mutex;

		HashMap<NotificationID, Notification> m_notificationTable;
		//
		////////////////////////////////////////////////////////////////
	};
}

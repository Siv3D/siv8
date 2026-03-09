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

# include "CNotifications.hpp"
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Notifications/CNotifications.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>
# include <ThirdParty/WinToast/wintoastlib.h>

namespace s3d
{
	using namespace WinToastLib;

	// シングルトンである CNotifications のインスタンスが生存しているかどうかを示すフラグ
	static std::atomic<bool> g_CNotificationsAlive{ false };

	class NotificationHandler final : public IWinToastHandler
	{
	public:

		NotificationHandler(CNotifications* pNotifications, const NotificationID id, const Array<NotificationAction>& actions)
			: m_pNotifications{ pNotifications }
			, m_id{ id }
			, m_actionIDs(actions.map([](const NotificationAction& action) { return action.id; }))
		{
			LOG_DEBUG(fmt::format("NotificationHandler::NotificationHandler() [id: {}, actions: {}]", id, m_actionIDs.size()));
		}

		void toastActivated() const override
		{
			if (g_CNotificationsAlive)
			{
				LOG_DEBUG(fmt::format("NotificationHandler::toastActivated() [id: {}]", m_id));

				m_pNotifications->enqueueResponse(m_id, NotificationResponseType::DefaultActivated);
			}
		}

		void toastActivated(const int actionIndex) const override
		{
			if (g_CNotificationsAlive)
			{
				LOG_DEBUG(fmt::format("NotificationHandler::toastActivated(int) [id: {}, actionIndex: {}]", m_id, actionIndex));
			
				if (InRange(actionIndex, 0, static_cast<int>(m_actionIDs.size())))
				{
					m_pNotifications->enqueueResponse(m_id, NotificationResponseType::ActionActivated, m_actionIDs[actionIndex]);
				}
				else
				{
					m_pNotifications->enqueueResponse(m_id, NotificationResponseType::DefaultActivated);
				}
			}
		}

		void toastActivated(const std::wstring) const override // 基本的には呼ばれない
		{
			if (g_CNotificationsAlive)
			{
				LOG_DEBUG(fmt::format("NotificationHandler::toastActivated(wstring) [id: {}]", m_id));

				m_pNotifications->enqueueResponse(m_id, NotificationResponseType::DefaultActivated);
			}
		}

		void toastDismissed(const WinToastDismissalReason state) const override
		{
			if (g_CNotificationsAlive)
			{
				LOG_DEBUG(fmt::format("NotificationHandler::toastDismissed() [id: {}, state: {}]", m_id, FromEnum(state)));

				switch (state)
				{
				case IWinToastHandler::UserCanceled:
				case IWinToastHandler::ApplicationHidden:
				case IWinToastHandler::TimedOut:
				default:
					m_pNotifications->enqueueResponse(m_id, NotificationResponseType::Dismissed);
					break;
				}
			}
		}

		void toastFailed() const override
		{
			if (g_CNotificationsAlive)
			{
				LOG_DEBUG(fmt::format(U"ToastHandler::toastFailed() [index: {}]", m_id));

				m_pNotifications->enqueueResponse(m_id, NotificationResponseType::Failed);
			}
		}

	private:

		CNotifications* m_pNotifications = nullptr;

		NotificationID m_id = 0;

		Array<String> m_actionIDs;
	};

	CNotifications::CNotifications()
	{
		g_CNotificationsAlive = true;
	}

	CNotifications::~CNotifications()
	{
		LOG_SCOPED_DEBUG("CNotifications::~CNotifications()");

		g_CNotificationsAlive = false; // 以降のイベントは捨てる

		dismissAll();
	}

	void CNotifications::init()
	{
		LOG_SCOPED_DEBUG("CNotifications::init()");
	}

	NotificationAvailability CNotifications::getAvailability()
	{
		if (not m_availability)
		{
			if (WinToast::isCompatible())
			{
				const std::wstring appName = FileSystem::BaseName(FileSystem::GetExecutablePath()).toWstr();
				WinToast::instance()->setAppName(appName);

				const auto aumi = WinToast::configureAUMI(L"Siv3D", appName, L"app");
				WinToast::instance()->setAppUserModelId(aumi);

				if (WinToast::instance()->initialize(nullptr))
				{
					m_availability = NotificationAvailability::Available;
				}
				else
				{
					m_availability = NotificationAvailability::Unavailable;
				}
			}
			else
			{
				m_availability = NotificationAvailability::Unavailable;
			}
		}

		return *m_availability;
	}

	void CNotifications::requestPermission() {}

	Optional<NotificationID> CNotifications::show(const NotificationRequest& request)
	{
		if (getAvailability() != NotificationAvailability::Available)
		{
			return none;
		}

		WinToastTemplate templ = WinToastTemplate(WinToastTemplate::ImageAndText02);
		
		if (request.imagePath && FileSystem::Exists(request.imagePath))
		{
			templ.setImagePath(Unicode::ToWstring(FileSystem::FullPath(request.imagePath)));
		}
		
		templ.setTextField(Unicode::ToWstring(request.title), WinToastTemplate::FirstLine);
		templ.setTextField(Unicode::ToWstring(request.body), WinToastTemplate::SecondLine);
		
		for (const auto& action : request.actions)
		{
			templ.addAction(Unicode::ToWstring(action.label));
		}

		templ.setAudioOption(request.playSound
			? WinToastLib::WinToastTemplate::AudioOption::Default
			: WinToastLib::WinToastTemplate::AudioOption::Silent);

		const NotificationID id = m_nextID.fetch_add(1);

		WinToast::WinToastError error = WinToast::NoError;
		NotificationHandler* handler = new NotificationHandler(this, id, request.actions);
		
		const int64 platformID = WinToast::instance()->showToast(templ, handler, &error); // handler の所有権は WinToast に移る
		if (platformID == -1)
		{
			LOG_FAIL(fmt::format("Failed to display the notification. WinToast returned an error: {}", Unicode::FromWstring(WinToast::strerror(error))));
			return none;
		}

		{
			std::lock_guard lock{ m_mutex };
			m_entries.emplace(id, Entry{ .platformID = platformID });
		}

		return id;
	}

	void CNotifications::dismiss(const NotificationID id)
	{
		if (getAvailability() != NotificationAvailability::Available)
		{
			return;
		}

		int64 platformID = -1;
		{
			std::lock_guard lock{ m_mutex };

			if (const auto it = m_entries.find(id);
				it != m_entries.end())
			{
				platformID = it->second.platformID;
			}
			else
			{
				return;
			}
		}

		WinToast::instance()->hideToast(platformID);
	}

	void CNotifications::dismissAll()
	{
		if (getAvailability() != NotificationAvailability::Available)
		{
			return;
		}

		WinToast::instance()->clear();
	}

	Array<NotificationResponse> CNotifications::extractResponses()
	{
		std::lock_guard lock{ m_mutex };
		return std::exchange(m_responseQueue, {});
	}

	void CNotifications::enqueueResponse(const NotificationID id, const NotificationResponseType responseType)
	{
		std::lock_guard lock{ m_mutex };
		m_responseQueue.push_back(NotificationResponse{ id, responseType });
	}

	void CNotifications::enqueueResponse(const NotificationID id, const NotificationResponseType responseType, const String& actionID)
	{
		std::lock_guard lock{ m_mutex };
		m_responseQueue.push_back(NotificationResponse{ id, responseType, actionID });
	}
}

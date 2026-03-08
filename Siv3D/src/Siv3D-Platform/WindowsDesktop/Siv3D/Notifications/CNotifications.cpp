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

namespace s3d
{
	using namespace WinToastLib;

	class NotificationHandler final : public IWinToastHandler
	{
	public:

		NotificationHandler(const Array<NotificationAction>& actions)
			: m_actionIDs(actions.map([](const NotificationAction& action) { return action.id; })) {}

		void setID(const NotificationID id)
		{
			LOG_DEBUG(fmt::format("NotificationHandler::setID() [id: {}]", id));
			m_id = id;
		}

		void toastActivated() const override
		{
			if (not Siv3DEngine::isNull())
			{
				LOG_DEBUG(fmt::format("NotificationHandler::toastActivated() [id: {}]", m_id));

				if (auto p = dynamic_cast<CNotifications*>(SIV3D_ENGINE(Notifications)))
				{
					p->onStateUpdate(m_id, CNotifications::Notification::State::Activated);
				}
			}
		}

		void toastActivated(const int actionIndex) const override
		{
			if (not Siv3DEngine::isNull())
			{
				LOG_DEBUG(fmt::format("NotificationHandler::toastActivated(int) [id: {}, actionIndex: {}]", m_id, actionIndex));

				if (auto p = dynamic_cast<CNotifications*>(SIV3D_ENGINE(Notifications)))
				{
					if ((0 <= actionIndex) && (actionIndex < m_actionIDs.size()))
					{
						p->onStateUpdate(m_id, CNotifications::Notification::State::Activated, m_actionIDs[actionIndex]);
					}
					else
					{
						p->onStateUpdate(m_id, CNotifications::Notification::State::Activated);
					}
				}
			}
		}

		void toastActivated(const std::wstring) const override {} // 基本的には呼ばれない

		void toastDismissed(const WinToastDismissalReason state) const override
		{
			if (not Siv3DEngine::isNull())
			{
				LOG_DEBUG(fmt::format("NotificationHandler::toastDismissed() [id: {}, state: {}]", m_id, FromEnum(state)));

				if (auto p = dynamic_cast<CNotifications*>(SIV3D_ENGINE(Notifications)))
				{
					switch (state)
					{
					case IWinToastHandler::UserCanceled:
						p->onStateUpdate(m_id, CNotifications::Notification::State::UserCanceled);
						break;
					case IWinToastHandler::ApplicationHidden:
						p->onStateUpdate(m_id, CNotifications::Notification::State::ApplicationHidden);
						break;
					case IWinToastHandler::TimedOut:
						p->onStateUpdate(m_id, CNotifications::Notification::State::TimedOut);
						break;
					default:
						break;
					}
				}
			}
		}

		void toastFailed() const override
		{
			if (not Siv3DEngine::isNull())
			{
				LOG_DEBUG(fmt::format(U"ToastHandler::toastFailed() [index: {}]", m_id));

				if (auto p = dynamic_cast<CNotifications*>(SIV3D_ENGINE(Notifications)))
				{
					p->onStateUpdate(m_id, CNotifications::Notification::State::Error);
				}
			}
		}

	private:

		Array<String> m_actionIDs;

		NotificationID m_id = -1;
	};

	CNotifications::~CNotifications()
	{
		LOG_SCOPED_DEBUG("CNotifications::~CNotifications()");

		dismissAll();
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

	bool CNotifications::requestPermission()
	{
		return (getAvailability() == NotificationAvailability::Available);
	}

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

		templ.setAudioOption(request.playSound ? WinToastLib::WinToastTemplate::AudioOption::Default : WinToastLib::WinToastTemplate::AudioOption::Silent);

		WinToast::WinToastError error = WinToast::NoError;
		NotificationHandler* handler = new NotificationHandler(request.actions);
		const NotificationID notificationID = WinToast::instance()->showToast(templ, handler, &error); // handler の所有権は WinToast に移る

		if (notificationID != -1)
		{
			handler->setID(notificationID);

			std::lock_guard lock{ m_mutex };
			m_notificationTable.emplace(notificationID, Notification{ {}, Notification::State::Shown});
		}

		return notificationID;
	}

	void CNotifications::dismiss(const NotificationID id)
	{
		if (getAvailability() != NotificationAvailability::Available)
		{
			return;
		}

		WinToast::instance()->hideToast(id);
	}

	void CNotifications::dismissAll()
	{
		if (getAvailability() != NotificationAvailability::Available)
		{
			return;
		}

		WinToast::instance()->clear();
	}

	Array<NotificationResponse> CNotifications::drainResponses()
	{
		return{};
	}

	void CNotifications::onStateUpdate(const NotificationID id, const Notification::State state)
	{
		std::lock_guard lock{ m_mutex };

		if (auto it = m_notificationTable.find(id);
			it != m_notificationTable.end())
		{
			it->second.state = state;
		}
	}

	void CNotifications::onStateUpdate(const NotificationID id, const Notification::State state, const String& actionID)
	{
		std::lock_guard lock{ m_mutex };

		if (auto it = m_notificationTable.find(id); 
			it != m_notificationTable.end())
		{
			it->second.state = state;
			it->second.actionID = actionID;
		}
	}
}

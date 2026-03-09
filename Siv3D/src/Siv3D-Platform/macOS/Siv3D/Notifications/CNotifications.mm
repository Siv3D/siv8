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
# include <UserNotifications/UserNotifications.h>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		static NSString* ToNSString(const String& s)
		{
			return [NSString stringWithUTF8String:Unicode::ToUTF8(s).c_str()];
		}

		static String ToSiv3DString(NSString* s)
		{
			if (s == nil)
			{
				return{};
			}

			return Unicode::FromUTF8([s UTF8String]);
		}

		static NotificationAvailability ToAvailability(const UNAuthorizationStatus status)
		{
			switch (status)
			{
			case UNAuthorizationStatusNotDetermined:
				return NotificationAvailability::NotDetermined;

			case UNAuthorizationStatusDenied:
				return NotificationAvailability::Denied;

			case UNAuthorizationStatusAuthorized:
				return NotificationAvailability::Available;

			default:
				return NotificationAvailability::Unavailable;
			}
		}
	}

	class CNotifications;
}

@interface Siv3DNotificationCenterDelegate : NSObject <UNUserNotificationCenterDelegate>
{
@private
	s3d::CNotifications* _pNotifications;
}
- (instancetype)initWithNotifications:(s3d::CNotifications*)pNotifications;
@end

@implementation Siv3DNotificationCenterDelegate

- (instancetype)initWithNotifications:(s3d::CNotifications*)pNotifications
{
	self = [super init];
	if (self)
	{
		_pNotifications = pNotifications;
	}
	return self;
}

- (void)userNotificationCenter:(UNUserNotificationCenter*)center
	   didReceiveNotificationResponse:(UNNotificationResponse*)response
				withCompletionHandler:(void (^)(void))completionHandler
{
	const s3d::String requestIdentifier = s3d::Unicode::FromUTF8([[response.notification.request.identifier description] UTF8String]);

	if (const auto id = _pNotifications->findIDFromRequestIdentifier(requestIdentifier))
	{
		NSString* actionIdentifier = response.actionIdentifier;

		if ([actionIdentifier isEqualToString:UNNotificationDefaultActionIdentifier])
		{
			_pNotifications->enqueueResponse(*id, s3d::NotificationResponseType::DefaultActivated);
		}
		else if ([actionIdentifier isEqualToString:UNNotificationDismissActionIdentifier])
		{
			_pNotifications->enqueueResponse(*id, s3d::NotificationResponseType::Dismissed);
		}
		else
		{
			_pNotifications->enqueueResponse(*id,
				s3d::NotificationResponseType::ActionActivated,
				s3d::Unicode::FromUTF8([actionIdentifier UTF8String]));
		}
	}

	if (completionHandler)
	{
		completionHandler();
	}
}

- (void)userNotificationCenter:(UNUserNotificationCenter*)center
	   willPresentNotification:(UNNotification*)notification
		 withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler
{
	if (completionHandler)
	{
# ifdef __MAC_11_0
		completionHandler(UNNotificationPresentationOptionBanner
			| UNNotificationPresentationOptionList
			| UNNotificationPresentationOptionSound);
# else
		completionHandler(UNNotificationPresentationOptionAlert
			| UNNotificationPresentationOptionSound);
# endif
	}
}

@end

namespace s3d
{
	CNotifications::CNotifications() = default;

	CNotifications::~CNotifications()
	{
		LOG_SCOPED_DEBUG("CNotifications::~CNotifications()");

		dismissAll();

		if (m_delegate)
		{
			UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

			if (center.delegate == (__bridge id<UNUserNotificationCenterDelegate>)m_delegate)
			{
				center.delegate = nil;
			}

			CFBridgingRelease(m_delegate);
			m_delegate = nullptr;
		}
	}

	void CNotifications::init()
	{
		LOG_SCOPED_DEBUG("CNotifications::init()");
		
		UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

		Siv3DNotificationCenterDelegate* delegate
			= [[Siv3DNotificationCenterDelegate alloc] initWithNotifications:this];

		center.delegate = delegate;
		m_delegate = CFBridgingRetain(delegate);

		m_availability = NotificationAvailability::NotDetermined;
		refreshAvailabilityAsync();
	}

	void CNotifications::refreshAvailabilityAsync()
	{
		UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

		[center getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings* _Nonnull settings)
		{
			this->updateAvailability(ToAvailability(settings.authorizationStatus));
		}];
	}

	void CNotifications::updateAvailability(const NotificationAvailability availability)
	{
		std::lock_guard lock{ m_mutex };
		m_availability = availability;
	}

	NotificationAvailability CNotifications::getAvailability()
	{
		{
			std::lock_guard lock{ m_mutex };

			if (m_availability)
			{
				return *m_availability;
			}
		}

		refreshAvailabilityAsync();
		return NotificationAvailability::NotDetermined;
	}

	void CNotifications::requestPermission()
	{
		UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

		UNAuthorizationOptions options = (UNAuthorizationOptionAlert | UNAuthorizationOptionSound);

		[center requestAuthorizationWithOptions:options
							  completionHandler:^(BOOL granted, NSError* _Nullable error)
		{
			if (error)
			{
				LOG_FAIL(fmt::format("requestAuthorizationWithOptions failed: {}",
					Unicode::FromUTF8([[error localizedDescription] UTF8String])));

				this->updateAvailability(NotificationAvailability::Unavailable);
				return;
			}

			this->updateAvailability(granted
				? NotificationAvailability::Available
				: NotificationAvailability::Denied);
		}];
	}

	Optional<NotificationID> CNotifications::show(const NotificationRequest& request)
	{
		const NotificationAvailability availability = getAvailability();

		if ((availability == NotificationAvailability::Unavailable)
			|| (availability == NotificationAvailability::Denied))
		{
			return none;
		}

		const NotificationID id = m_nextID.fetch_add(1);

		const String requestIdentifier = U"s3d.notification.request.{}"_fmt(id);
		const String categoryIdentifier = U"s3d.notification.category.{}"_fmt(id);

		auto addRequest = [this, id](UNNotificationRequest* nativeRequest)
		{
			[[UNUserNotificationCenter currentNotificationCenter]
				addNotificationRequest:nativeRequest
				withCompletionHandler:^(NSError* _Nullable error)
			{
				if (error)
				{
					LOG_FAIL(fmt::format("addNotificationRequest failed: {}",
						Unicode::FromUTF8([[error localizedDescription] UTF8String])));
					this->enqueueResponse(id, NotificationResponseType::Failed);
				}
			}];
		};

		UNMutableNotificationContent* content = [[UNMutableNotificationContent alloc] init];
		content.title = ToNSString(request.title);
		content.body = ToNSString(request.body);

		if (request.playSound)
		{
			content.sound = [UNNotificationSound defaultSound];
		}

		Array<NotificationAction> actions = request.actions;

		// macOS / UserNotifications 系ではアクション数に制限があるため保守的に 4 件まで
		if (actions.size() > 4)
		{
			actions.resize(4);
			LOG_WARN("Notification actions were truncated to 4 items on macOS.");
		}

		// 元の画像ファイルが削除される挙動になるため、macOS 版では imagePath は無効化
//		if (request.imagePath && FileSystem::Exists(request.imagePath))
//		{
//			NSString* path = ToNSString(FileSystem::FullPath(request.imagePath));
//			NSURL* fileURL = [NSURL fileURLWithPath:path];
//
//			NSError* attachmentError = nil;
//			UNNotificationAttachment* attachment
//				= [UNNotificationAttachment attachmentWithIdentifier:@"image"
//																URL:fileURL
//															options:nil
//															  error:&attachmentError];
//
//			if (attachment)
//			{
//				content.attachments = @[ attachment ];
//			}
//			else if (attachmentError)
//			{
//				LOG_WARN(fmt::format("Failed to attach image to notification: {}",
//					Unicode::FromUTF8([[attachmentError localizedDescription] UTF8String])));
//			}
//		}

		if (not actions.isEmpty())
		{
			content.categoryIdentifier = ToNSString(categoryIdentifier);
		}

		UNNotificationRequest* nativeRequest
			= [UNNotificationRequest requestWithIdentifier:ToNSString(requestIdentifier)
												   content:content
												   trigger:nil];

		{
			std::lock_guard lock{ m_mutex };

			m_entries.emplace(id, Entry{
				.requestIdentifier = requestIdentifier,
				.categoryIdentifier = categoryIdentifier,
				.actionIDs = actions.map([](const NotificationAction& action) { return action.id; })
			});

			m_requestIdentifierToID.emplace(requestIdentifier, id);
		}

		if (not actions.isEmpty())
		{
			NSMutableArray<UNNotificationAction*>* nativeActions = [NSMutableArray arrayWithCapacity:actions.size()];

			for (const auto& action : actions)
			{
				UNNotificationAction* nativeAction
					= [UNNotificationAction actionWithIdentifier:ToNSString(action.id)
														   title:ToNSString(action.label)
														 options:UNNotificationActionOptionNone];
				[nativeActions addObject:nativeAction];
			}

			UNNotificationCategory* category
				= [UNNotificationCategory categoryWithIdentifier:ToNSString(categoryIdentifier)
														actions:nativeActions
											  intentIdentifiers:@[]
														options:UNNotificationCategoryOptionCustomDismissAction];

			UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

			[center getNotificationCategoriesWithCompletionHandler:^(NSSet<UNNotificationCategory*>* _Nonnull categories)
			{
				NSMutableSet<UNNotificationCategory*>* merged = [categories mutableCopy];
				[merged addObject:category];
				[center setNotificationCategories:merged];

				addRequest(nativeRequest);
			}];
		}
		else
		{
			addRequest(nativeRequest);
		}

		return id;
	}

	void CNotifications::dismiss(const NotificationID id)
	{
		String requestIdentifier;

		{
			std::lock_guard lock{ m_mutex };

			if (const auto it = m_entries.find(id);
				it != m_entries.end())
			{
				requestIdentifier = it->second.requestIdentifier;
			}
			else
			{
				return;
			}
		}

		NSArray<NSString*>* identifiers = @[ ToNSString(requestIdentifier) ];
		UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

		[center removePendingNotificationRequestsWithIdentifiers:identifiers];
		[center removeDeliveredNotificationsWithIdentifiers:identifiers];
	}

	void CNotifications::dismissAll()
	{
		UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];

		[center removeAllPendingNotificationRequests];
		[center removeAllDeliveredNotifications];
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

	Optional<NotificationID> CNotifications::findIDFromRequestIdentifier(const String& requestIdentifier)
	{
		std::lock_guard lock{ m_mutex };

		if (const auto it = m_requestIdentifierToID.find(requestIdentifier);
			it != m_requestIdentifierToID.end())
		{
			return it->second;
		}

		return none;
	}
}

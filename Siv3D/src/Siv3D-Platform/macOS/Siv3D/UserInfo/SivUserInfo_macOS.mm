//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/UserInfo.hpp>
# include <Siv3D/Unicode.hpp>
# import  <Cocoa/Cocoa.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static String ComputerName()
		{
			@autoreleasepool
			{
				NSString *localizedName = [[NSHost currentHost] localizedName];
				
				if (localizedName)
				{
					return Unicode::FromUTF8(std::string([localizedName UTF8String]));
				}
				else
				{
					return{};
				}
			}
		}

		[[nodiscard]]
		static String UserName()
		{
			@autoreleasepool
			{
				NSString *userName = NSUserName();
				return Unicode::FromUTF8(std::string([userName UTF8String]));
			}
		}

		[[nodiscard]]
		static String FullUserName()
		{
			@autoreleasepool
			{
				NSString *fullUserName = NSFullUserName();
				return Unicode::FromUTF8(std::string([fullUserName UTF8String]));
			}
		}

		[[nodiscard]]
		static String DefaultLocale()
		{
			@autoreleasepool
			{
				CFLocaleRef locale = CFLocaleCopyCurrent();
				
				CFStringRef languageCodeStr = (CFStringRef)CFLocaleGetValue(locale, kCFLocaleLanguageCode);
				NSString *nsLanguageCode = (__bridge NSString*)languageCodeStr;
				std::string languageCode([nsLanguageCode UTF8String]);
				
				CFStringRef countryCodeStr = (CFStringRef)CFLocaleGetValue(locale, kCFLocaleCountryCode);
				NSString *nsCountryCode = (__bridge NSString*)countryCodeStr;
				std::string countryCode([nsCountryCode UTF8String]);
				
				CFRelease(locale);
				
				if (languageCode.empty())
				{
					return U"en-US";
				}
				else
				{
					return (Unicode::FromAscii(languageCode) + U"-" + Unicode::FromAscii(countryCode));
				}
			}
		}

		[[nodiscard]]
		static String DefaultLanguage()
		{
			@autoreleasepool
			{
				NSArray<NSString*>* preferredLanguages = [NSLocale preferredLanguages];
				
				if (0 < preferredLanguages.count)
				{
					NSString *language = preferredLanguages[0];
					return Unicode::FromAscii(std::string([language UTF8String]));
				}
				else
				{
					// Fallback to a default value if preferredLanguages is empty
					return U"en-US";
				}
			}
		}

		[[nodiscard]]
		static bool IsRunningInXcode_impl()
		{
			return (std::getenv("__XCODE_BUILT_PRODUCTS_DIR_PATHS") != nullptr);
		}
	}

	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetUserInfo
		//
		////////////////////////////////////////////////////////////////

		const UserInfo& GetUserInfo()
		{
			static const UserInfo userInfo = []()
			{
				UserInfo info;
				info.computerName		= ComputerName();
				info.userName			= UserName();
				info.fullUserName		= FullUserName();
				info.defaultLocale		= DefaultLocale();
				info.defaultLanguage	= DefaultLanguage();
				return info;
			}();

			return userInfo;
		}

		////////////////////////////////////////////////////////////////
		//
		//	IsRunningInVisualStudio
		//
		////////////////////////////////////////////////////////////////

		bool IsRunningInVisualStudio()
		{
			return false;
		}
	}
}

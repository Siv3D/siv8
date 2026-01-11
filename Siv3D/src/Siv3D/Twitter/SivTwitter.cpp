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

# include <Siv3D/Twitter.hpp>
# include <Siv3D/URL.hpp>
# include <Siv3D/IntToString.hpp>
# include <Siv3D/PercentEncode.hpp>
# include <Siv3D/System.hpp>

namespace s3d
{
	namespace Twitter
	{
		////////////////////////////////////////////////////////////////
		//
		//	OpenUserPage
		//
		////////////////////////////////////////////////////////////////

		bool OpenUserPage(const StringView screenName)
		{
			const URL url = (U"https://x.com/" + screenName);
			return System::LaunchBrowser(url);
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenTweetWindow
		//
		////////////////////////////////////////////////////////////////

		bool OpenTweetWindow(const StringView text)
		{
			const URL url = (U"https://x.com/intent/tweet?text=" + PercentEncode(text));
			return System::LaunchBrowser(url);
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenSearchWindow
		//
		////////////////////////////////////////////////////////////////

		bool OpenSearchWindow(const StringView query)
		{
			const URL url = (U"https://x.com/search?q=" + PercentEncode(query));
			return System::LaunchBrowser(url);
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenHashtagWindow
		//
		////////////////////////////////////////////////////////////////

		bool OpenHashtagWindow(const StringView hashtag)
		{
			return OpenSearchWindow(U"#" + hashtag);
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenMiniProfileWindow
		//
		////////////////////////////////////////////////////////////////

		bool OpenMiniProfileWindow(const StringView screenName)
		{
			const URL url = (U"https://x.com/intent/user?screen_name=" + screenName);
			return System::LaunchBrowser(url);
		}

		bool OpenMiniProfileWindow(const uint64 userID)
		{
			const URL url = (U"https://x.com/intent/user?user_id=" + ToString(userID));
			return System::LaunchBrowser(url);
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenFollowWindow
		//
		////////////////////////////////////////////////////////////////

		bool OpenFollowWindow(const StringView screenName)
		{
			const URL url = (U"https://x.com/intent/follow?screen_name=" + screenName);
			return System::LaunchBrowser(url);
		}

		bool OpenFollowWindow(const uint64 userID)
		{
			const URL url = (U"https://x.com/intent/follow?user_id=" + ToString(userID));
			return System::LaunchBrowser(url);
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenLikeWindow
		//
		////////////////////////////////////////////////////////////////

		bool OpenLikeWindow(const uint64 tweetID)
		{
			const URL url = (U"https://x.com/intent/like?tweet_id=" + ToString(tweetID));
			return System::LaunchBrowser(url);
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenRepostWindow
		//
		////////////////////////////////////////////////////////////////

		bool OpenRepostWindow(const uint64 tweetID)
		{
			const URL url = (U"https://x.com/intent/retweet?tweet_id=" + ToString(tweetID));
			return System::LaunchBrowser(url);
		}
	}
}

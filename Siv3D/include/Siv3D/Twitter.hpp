//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "StringView.hpp"

namespace s3d
{
	/// @brief X (旧 Twitter) に関する機能
	namespace Twitter
	{
		////////////////////////////////////////////////////////////////
		//
		//	OpenUserPage
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したユーザのトップページをユーザの既定のブラウザで開きます。
		/// @param screenName スクリーンネーム（`@` を除いた文字列）
		/// @return ブラウザを起動できたら true, それ以外の場合は false
		bool OpenUserPage(StringView screenName);

		////////////////////////////////////////////////////////////////
		//
		//	OpenTweetWindow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 投稿ページをユーザの既定のブラウザで開きます。
		/// @param text 投稿するテキスト
		/// @return ブラウザを起動できたら true, それ以外の場合は false
		bool OpenTweetWindow(StringView text);

		////////////////////////////////////////////////////////////////
		//
		//	OpenSearchWindow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 検索ページをユーザの既定のブラウザで開きます。
		/// @param query 検索クエリ（例: `Siv3D`, `from:username`, `#hashtag` など）
		/// @return ブラウザを起動できたら true, それ以外の場合は false
		bool OpenSearchWindow(StringView query);

		////////////////////////////////////////////////////////////////
		//
		//	OpenHashtagWindow
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief 指定したハッシュタグの検索ページをユーザの既定のブラウザで開きます。
		/// @param hashtag ハッシュタグ（`#` を除いた文字列）
		/// @return ブラウザを起動できたら true, それ以外の場合は false
		bool OpenHashtagWindow(StringView hashtag);

		////////////////////////////////////////////////////////////////
		//
		//	OpenMiniProfileWindow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したユーザのミニプロフィールをユーザの既定のブラウザで開きます。
		/// @param screenName スクリーンネーム（`@` を除いた文字列）
		/// @return ブラウザを起動できたら true, それ以外の場合は false
		bool OpenMiniProfileWindow(StringView screenName);
		
		/// @brief 指定したユーザのミニプロフィールをユーザの既定のブラウザで開きます。
		/// @param userID ユーザ ID
		/// @return ブラウザを起動できたら true, それ以外の場合は false
		bool OpenMiniProfileWindow(uint64 userID);

		////////////////////////////////////////////////////////////////
		//
		//	OpenFollowWindow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したユーザのフォロー確認ページをユーザの既定のブラウザで開きます。
		/// @param screenName スクリーンネーム（`@` を除いた文字列）
		/// @return ブラウザを起動できたら true, それ以外の場合は false
		bool OpenFollowWindow(StringView screenName);

		/// @brief 指定したユーザのフォロー確認ページをユーザの既定のブラウザで開きます。
		/// @param userID ユーザ ID
		/// @return ブラウザを起動できたら true, それ以外の場合は false
		bool OpenFollowWindow(uint64 userID);

		////////////////////////////////////////////////////////////////
		//
		//	OpenLikeWindow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したポスト（ツイート）のいいね確認ページをユーザの既定のブラウザで開きます。
		/// @param tweetID ポスト（ツイート）ID
		/// @return ブラウザを起動できたら true, それ以外の場合は false
		bool OpenLikeWindow(uint64 tweetID);

		////////////////////////////////////////////////////////////////
		//
		//	OpenRepostWindow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したポスト（ツイート）のリポスト（リツイート）確認ページをユーザの既定のブラウザで開きます。
		/// @param tweetID ポスト（ツイート）ID
		/// @return ブラウザを起動できたら true, それ以外の場合は false
		bool OpenRepostWindow(uint64 tweetID);
	}
}

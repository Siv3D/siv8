//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"

namespace s3d
{
	/// @brief 特殊フォルダ | Special folder
	/// @remark `FileSystem::GetFolderPath()` で使用します。 | Used in `FileSystem::GetFolderPath()`.
	enum class SpecialFolder
	{
		/// @brief デスクトップ | Desktop
		Desktop,

		/// @brief ドキュメント | Documents
		Documents,

		/// @brief ローカル・アプリケーション・データ（キャッシュ） | Local application data (cache)
		LocalAppData,

		/// @brief ピクチャー | Pictures
		Pictures,

		/// @brief ミュージック | Music
		Music,

		/// @brief ビデオ | Videos
		Videos,

		/// @brief キャッシュ | Cache
		Caches = LocalAppData,

		/// @brief ムービー（ビデオ） | Movies (Videos)
		Movies = Videos,

		/// @brief システムフォント | System fonts
		SystemFonts,

		/// @brief ローカルフォント | Local fonts
		LocalFonts,

		/// @brief ユーザーフォント | User fonts
		UserFonts,

		/// @brief ユーザープロファイル | User profile
		UserProfile,

		/// @brief アプリケーション | Applications
		ProgramFiles,
	};
}

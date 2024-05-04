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

# pragma once
# include <Siv3D/Common.hpp>
# include <string>

namespace s3d
{
	/// @brief エンジン未初期化でも使えるメッセージボックス
	namespace FreestandingMessageBox
	{
		/// @brief メッセージボックスを表示します。
		/// @param text メッセージボックスに表示するテキスト
		void ShowInfo(std::string_view text);

		/// @brief エラーメッセージボックスを表示します。
		/// @param text メッセージボックスに表示するテキスト
		void ShowError(std::string_view text);
	}
}

//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <string>
# include "Common.hpp"

namespace s3d
{
	using StringView = std::u32string_view;

	/// @brief 文字コードの変換
	namespace Unicode
	{
		/// @brief String を std::string に変換します。
		/// @param s 変換する文字列
		/// @return 変換された文字列
		[[nodiscard]]
		std::string Narrow(StringView s);

		/// @brief String を std::wstring に変換します。
		/// @param s 変換する文字列
		/// @return 変換された文字列
		[[nodiscard]]
		std::wstring ToWstring(StringView s);
	}
}

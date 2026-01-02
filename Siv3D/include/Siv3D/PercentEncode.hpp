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
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	PercentEncode
	//
	////////////////////////////////////////////////////////////////

	/// @brief 文字列をパーセントエンコードします。
	/// @param s エンコードする文字列
	/// @return パーセントエンコードされた文字列
	[[nodiscard]]
	std::string PercentEncode(const std::string_view s);

	/// @brief 文字列をパーセントエンコードします。
	/// @param s エンコードする文字列
	/// @return パーセントエンコードされた文字列
	[[nodiscard]]
	String PercentEncode(const StringView s);
}

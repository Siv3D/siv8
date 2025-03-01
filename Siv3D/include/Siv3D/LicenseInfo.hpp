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
# include "Common.hpp"
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	LicenseInfo
	//
	////////////////////////////////////////////////////////////////

	/// @brief ライセンス情報
	/// @remark 消費メモリ節約のため、String ではなく UTF-8 文字列で保持します。
	struct LicenseInfo
	{
		/// @brief 見出し
		std::string title;

		/// @brief コピーライト表示
		std::string copyright;

		/// @brief 説明文
		std::string text;
	};
}

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
# include "Common.hpp"
# include <ThirdParty/EnumBitmask/EnumBitmask.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	OpenMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief ファイルのオープンモード | File open mode
	enum class OpenMode : uint8
	{
		/// @brief 新規作成 | Truncate
		Trunc	= (1 << 1),

		/// @brief 追加 | Append
		Append	= (1 << 2),
	};
	DEFINE_BITMASK_OPERATORS(OpenMode);
}

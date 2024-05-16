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
# include "Types.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	LetterCase
	//
	////////////////////////////////////////////////////////////////

	/// @brief アルファベットの大文字・小文字 | Letter case
	enum class LetterCase : uint8
	{
		/// @brief 小文字 | Lowercase
		Lower,

		/// @brief 大文字 | Uppercase
		Upper,
	};
}

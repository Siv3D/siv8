﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/String.hpp>
# include <Siv3D/BoolToString.hpp>

namespace s3d
{
	constexpr StringView BoolLS[2] = { U"false", U"true" };

	constexpr StringView BoolUS[2] = { U"FALSE", U"TRUE" };

	String ToString(const bool value)
	{
		return String(BoolLS[value]);
	}

	String ToString(const bool value, const LetterCase letterCase)
	{
		return String((letterCase == LetterCase::Lower) ? BoolLS[value] : BoolUS[value]);
	}
}
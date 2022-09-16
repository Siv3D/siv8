﻿//-----------------------------------------------
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

namespace s3d
{
	namespace Unicode
	{
		inline constexpr bool IsHighSurrogate(const char16 ch) noexcept
		{
			return ((0xD800 <= ch) && (ch <= 0xDBFF));
		}

		inline constexpr bool IsLowSurrogate(const char16 ch) noexcept
		{
			return ((0xDC00 <= ch) && (ch <= 0xDFFF));
		}
	}
}

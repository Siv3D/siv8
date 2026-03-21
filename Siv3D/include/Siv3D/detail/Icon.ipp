//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr Icon::Icon(const char32 _code) noexcept
		: code{ _code } {}

	inline namespace Literals
	{
		inline namespace IconLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_icon
			//
			////////////////////////////////////////////////////////////////

			constexpr Icon operator ""_icon(unsigned long long codePoint) noexcept
			{
				return Icon{ static_cast<char32>(codePoint) };
			}
		}
	}
}

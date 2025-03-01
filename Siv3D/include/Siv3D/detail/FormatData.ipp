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

namespace s3d
{		
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr FormatData::FormatData(const char32* s)
		: string{ s } {}

	constexpr FormatData::FormatData(const StringView s)
		: string{ s } {}

	constexpr FormatData::FormatData(const String& s)
		: string{ s } {}

	constexpr FormatData::FormatData(String&& s) noexcept
		: string{ std::move(s).str() } {}
}

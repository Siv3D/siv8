//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	constexpr FormatData::DecimalPlaces::DecimalPlaces(const int32 v) noexcept
		: value{ v } {}

	inline FormatData::FormatData(const char32* s)
		: string{ s } {}

	inline FormatData::FormatData(const StringView s)
		: string{ s } {}

	inline FormatData::FormatData(const String& s)
		: string{ s } {}

	inline FormatData::FormatData(String&& s) noexcept
		: string{ std::move(s) } {}

	constexpr FormatData::DecimalPlaces DecimalPlaces(const int32 width) noexcept
	{
		return FormatData::DecimalPlaces{ width };
	}

	namespace Literals
	{
		namespace DecimalPlaceLiterals
		{
			[[nodiscard]]
			constexpr FormatData::DecimalPlaces operator ""_dp(const unsigned long long width) noexcept
			{
				return FormatData::DecimalPlaces{ static_cast<int32>(width) };
			}
		}
	}
}

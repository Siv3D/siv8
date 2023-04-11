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
# include "String.hpp"

namespace s3d
{
	struct FormatData
	{
		struct DecimalPlaces
		{
			static constexpr int32 DefaultDecimalPlaces = 5;

			int32 value = DefaultDecimalPlaces;

			[[nodiscard]]
			DecimalPlaces() = default;

			[[nodiscard]]
			explicit constexpr DecimalPlaces(int32 v) noexcept;

			friend void Formatter(FormatData& formatData, DecimalPlaces decimalPlace);
		};

		[[nodiscard]]
		FormatData() = default;

		[[nodiscard]]
		explicit FormatData(const char32* s);

		[[nodiscard]]
		explicit FormatData(StringView s);

		[[nodiscard]]
		explicit FormatData(const String& s);

		[[nodiscard]]
		explicit FormatData(String&& s) noexcept;

		String string;

		DecimalPlaces decimalPlaces;
	};

	[[nodiscard]]
	constexpr FormatData::DecimalPlaces DecimalPlaces(int32 width) noexcept;

	inline namespace Literals
	{
		inline namespace DecimalPlaceLiterals
		{
			[[nodiscard]]
			constexpr FormatData::DecimalPlaces operator ""_dp(unsigned long long width) noexcept;
		}
	}
}

# include "detail/FormatData.ipp"

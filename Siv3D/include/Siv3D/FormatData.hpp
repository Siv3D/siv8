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
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	FormatData
	//
	////////////////////////////////////////////////////////////////

	struct FormatData
	{
		String string;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr FormatData() = default;

		[[nodiscard]]
		explicit constexpr FormatData(const char32* s);

		[[nodiscard]]
		explicit constexpr FormatData(StringView s);

		[[nodiscard]]
		explicit constexpr FormatData(const String& s);

		[[nodiscard]]
		explicit constexpr FormatData(String&& s) noexcept;
	};
}

# include "detail/FormatData.ipp"

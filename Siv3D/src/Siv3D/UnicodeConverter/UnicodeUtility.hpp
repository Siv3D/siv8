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
# include <Siv3D/StringView.hpp>

namespace s3d
{
	namespace detail
	{
		////////////////////////////////////////////////////////////////
		//
		//	UTF8_Length
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		size_t UTF8_Length(std::u16string_view s) noexcept;

		[[nodiscard]]
		size_t UTF8_Length(char32 codePoint) noexcept;

		[[nodiscard]]
		size_t UTF8_Length(StringView s) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UTF8_Encode
		//
		////////////////////////////////////////////////////////////////

		void UTF8_Encode(char8** s, char32 codePoint) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UTF16_Length
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		size_t UTF16_Length(std::string_view s) noexcept;

		[[nodiscard]]
		size_t UTF16_Length(char32 codePoint) noexcept;

		[[nodiscard]]
		size_t UTF16_Length(StringView s) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UTF16_Encode
		//
		////////////////////////////////////////////////////////////////

		void UTF16_Encode(char16** s, char32 codePoint) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UTF32_Length
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		size_t UTF32_Length(std::string_view s) noexcept;

		[[nodiscard]]
		size_t UTF32_Length(std::u16string_view s) noexcept;
	}
}

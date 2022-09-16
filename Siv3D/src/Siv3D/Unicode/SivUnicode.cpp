//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Unicode.hpp>
# include <ThirdParty/simdutf/simdutf.h>

namespace s3d
{
	namespace Unicode
	{
		String WidenAscii(const std::string_view asciiText)
		{
			return String(asciiText.begin(), asciiText.end());
		}

		String FromUTF8(const std::string_view s)
		{
			const size_t requiredLength = simdutf::utf32_length_from_utf8(s.data(), s.size());

		# if defined(__cpp_lib_string_resize_and_overwrite)

			String result;

			result.resize_and_overwrite(requiredLength, [&](char32* buf, size_t) {
				return simdutf::convert_utf8_to_utf32(s.data(), s.size(), buf);
				});

		# else

			String result(requiredLength, U'\0');

			if (0 == simdutf::convert_utf8_to_utf32(s.data(), s.size(), result.data()))
			{
				return{};
			}

		# endif

			return result;
		}

		String FromUTF16(const std::u16string_view s)
		{
			const size_t requiredLength = simdutf::utf32_length_from_utf16le(s.data(), s.size());

		# if defined(__cpp_lib_string_resize_and_overwrite)

			String result;

			result.resize_and_overwrite(requiredLength, [&](char32* buf, size_t) {
				return simdutf::convert_utf16le_to_utf32(s.data(), s.size(), buf);
				});

		# else

			String result(requiredLength, U'\0');

			if (0 == simdutf::convert_utf16le_to_utf32(s.data(), s.size(), result.data()))
			{
				return{};
			}

		# endif

			return result;
		}

		String FromUTF32(const std::u32string_view s)
		{
			return String{ s };
		}

		std::string NarrowAscii(const StringView asciiText)
		{
			std::string result(asciiText.length(), '\0');

			const char32* pSrc = asciiText.data();
			const char32* const pSrcEnd = (pSrc + asciiText.size());
			char* pDst = result.data();

			while (pSrc != pSrcEnd)
			{
				*pDst++ = static_cast<char>(*pSrc++);
			}

			return result;
		}

		std::string ToUTF8(const StringView s)
		{
			const size_t requiredLength = simdutf::utf8_length_from_utf32(s.data(), s.size());

		# if defined(__cpp_lib_string_resize_and_overwrite)

			std::string result;

			result.resize_and_overwrite(requiredLength, [&](char* buf, size_t) {
				return simdutf::convert_utf32_to_utf8(s.data(), s.size(), buf);
				});

		# else

			std::string result(requiredLength, '\0');

			if (0 == simdutf::convert_utf32_to_utf8(s.data(), s.size(), result.data()))
			{
				return{};
			}

		# endif

			return result;
		}

		std::u16string ToUTF16(const StringView s)
		{
			const size_t requiredLength = simdutf::utf16_length_from_utf32(s.data(), s.size());

		# if defined(__cpp_lib_string_resize_and_overwrite)

			std::u16string result;

			result.resize_and_overwrite(requiredLength, [&](char16* buf, size_t) {
				return simdutf::convert_utf32_to_utf16le(s.data(), s.size(), buf);
				});

		# else

			std::u16string result(requiredLength, u'\0');

			if (0 == simdutf::convert_utf32_to_utf16le(s.data(), s.size(), result.data()))
			{
				return{};
			}

		# endif

			return result;
		}

		std::u32string ToUTF32(const StringView s)
		{
			return std::u32string(s.begin(), s.end());
		}

		std::u16string UTF8ToUTF16(const std::string_view s)
		{
			const size_t requiredLength = simdutf::utf16_length_from_utf8(s.data(), s.size());

		# if defined(__cpp_lib_string_resize_and_overwrite)

			std::u16string result;

			result.resize_and_overwrite(requiredLength, [&](char16* buf, size_t) {
				return simdutf::convert_utf8_to_utf16le(s.data(), s.size(), buf);
				});

		# else

			std::u16string result(requiredLength, u'\0');

			if (0 == simdutf::convert_utf8_to_utf16le(s.data(), s.size(), result.data()))
			{
				return{};
			}

		# endif

			return result;
		}

		std::u32string UTF8ToUTF32(const std::string_view s)
		{
			const size_t requiredLength = simdutf::utf32_length_from_utf8(s.data(), s.size());

		# if defined(__cpp_lib_string_resize_and_overwrite)

			std::u32string result;

			result.resize_and_overwrite(requiredLength, [&](char32* buf, size_t) {
				return simdutf::convert_utf8_to_utf32(s.data(), s.size(), buf);
				});

		# else

			std::u32string result(requiredLength, u'\0');

			if (0 == simdutf::convert_utf8_to_utf32(s.data(), s.size(), result.data()))
			{
				return{};
			}

		# endif

			return result;
		}

		std::string UTF16ToUTF8(const std::u16string_view s)
		{
			const size_t requiredLength = simdutf::utf8_length_from_utf16le(s.data(), s.size());

		# if defined(__cpp_lib_string_resize_and_overwrite)

			std::string result;

			result.resize_and_overwrite(requiredLength, [&](char* buf, size_t) {
				return simdutf::convert_utf16le_to_utf8(s.data(), s.size(), buf);
				});

		# else

			std::string result(requiredLength, '\0');

			if (0 == simdutf::convert_utf16le_to_utf8(s.data(), s.size(), result.data()))
			{
				return{};
			}

		# endif

			return result;
		}

		std::u32string UTF16ToUTF32(const std::u16string_view s)
		{
			const size_t requiredLength = simdutf::utf32_length_from_utf16le(s.data(), s.size());

		# if defined(__cpp_lib_string_resize_and_overwrite)

			std::u32string result;

			result.resize_and_overwrite(requiredLength, [&](char32* buf, size_t) {
				return simdutf::convert_utf16le_to_utf32(s.data(), s.size(), buf);
				});

		# else

			std::u32string result(requiredLength, U'\0');

			if (0 == simdutf::convert_utf16le_to_utf32(s.data(), s.size(), result.data()))
			{
				return{};
			}

		# endif

			return result;
		}

		std::string UTF32ToUTF8(const std::u32string_view s)
		{
			const size_t requiredLength = simdutf::utf8_length_from_utf32(s.data(), s.size());

		# if defined(__cpp_lib_string_resize_and_overwrite)

			std::string result;

			result.resize_and_overwrite(requiredLength, [&](char* buf, size_t) {
				return simdutf::convert_utf32_to_utf8(s.data(), s.size(), buf);
				});

		# else

			std::string result(requiredLength, '\0');

			if (0 == simdutf::convert_utf32_to_utf8(s.data(), s.size(), result.data()))
			{
				return{};
			}

		# endif

			return result;
		}

		std::u16string UTF32ToUTF16(const std::u32string_view s)
		{
			const size_t requiredLength = simdutf::utf16_length_from_utf32(s.data(), s.size());

		# if defined(__cpp_lib_string_resize_and_overwrite)

			std::u16string result;

			result.resize_and_overwrite(requiredLength, [&](char16* buf, size_t) {
				return simdutf::convert_utf32_to_utf16le(s.data(), s.size(), buf);
				});

		# else

			std::u16string result(requiredLength, u'\0');

			if (0 == simdutf::convert_utf32_to_utf16le(s.data(), s.size(), result.data()))
			{
				return{};
			}

		# endif

			return result;
		}
	}
}

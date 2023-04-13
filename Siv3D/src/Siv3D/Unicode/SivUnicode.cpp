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

# include <Siv3D/Unicode.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/String.hpp>
# include <ThirdParty/simdutf/simdutf.h>

namespace s3d
{
	namespace Unicode
	{
		String WidenAscii(const std::string_view asciiText)
		{
			const size_t requiredLength = asciiText.size();

			if (requiredLength < 32) // 短い文字列は非 SIMD
			{
			# if defined(__cpp_lib_string_resize_and_overwrite)

				String result;

				result.resize_and_overwrite(asciiText.size(), [&](char32* buf, size_t n) -> size_t {

						for (const char ch : asciiText)
						{
							if (static_cast<uint8>(ch) <= uint8{ 0x7F })
							{
								*buf++ = ch;
							}
							else
							{
								return 0;
							}
						}

						return n;
					});

			# else

				String result(asciiText.size(), U'\0');

				char32* pDst = result.data();

				for (const char ch : asciiText)
				{
					if (static_cast<uint8>(ch) <= uint8{ 0x7F })
					{
						*pDst++ = ch;
					}
					else
					{
						return{};
					}
				}

			# endif

				return result;
			}
			else
			{
				if (not simdutf::validate_ascii(asciiText.data(), asciiText.size()))
				{
					return{};
				}

			# if defined(__cpp_lib_string_resize_and_overwrite)

				String result;

				result.resize_and_overwrite(requiredLength, [&](char32* buf, size_t) {
					return simdutf::convert_utf8_to_utf32(asciiText.data(), asciiText.size(), buf);
					});

			# else

				String result(requiredLength, U'\0');

				if (0 == simdutf::convert_utf8_to_utf32(asciiText.data(), asciiText.size(), result.data()))
				{
					return{};
				}

			# endif

				return result;
			}
		}

		String FromWstring(const std::wstring_view s)
		{
		# if SIV3D_PLATFORM(WINDOWS)

			static_assert(sizeof(wchar_t) == 2);

			const char16* pSrc = static_cast<const char16*>(static_cast<const void*>(s.data()));

			return FromUTF16(std::u16string_view(pSrc, s.size()));

		# else

			static_assert(sizeof(wchar_t) == 4);

			return String(s.begin(), s.end());

		# endif
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
		# if defined(__cpp_lib_string_resize_and_overwrite)

			std::string result;

			result.resize_and_overwrite(asciiText.size(), [&](char* buf, size_t n) -> size_t {

					for (const char32 ch : asciiText)
					{
						if (static_cast<uint32>(ch) <= 0x7Fu)
						{
							*buf++ = static_cast<char>(ch);
						}
						else
						{
							return 0;
						}
					}

					return n;
				});

		# else

			std::string result(asciiText.size(), '\0');

			char* pDst = result.data();

			for (const char32 ch : asciiText)
			{
				if (static_cast<uint32>(ch) <= 0x7Fu)
				{
					*pDst++ = static_cast<char>(ch);
				}
				else
				{
					return{};
				}
			}

		# endif

			return result;
		}

		std::wstring ToWstring(std::string_view s)
		{
		# if SIV3D_PLATFORM(WINDOWS)

			static_assert(sizeof(wchar_t) == sizeof(char16));
			
			const size_t requiredLength = simdutf::utf16_length_from_utf8(s.data(), s.size());

			std::wstring result;

			result.resize_and_overwrite(requiredLength, [&](wchar_t* buf, size_t) {
				return simdutf::convert_utf8_to_utf16le(s.data(), s.size(), static_cast<char16*>(static_cast<void*>(buf)));
				});

			return result;

		# else

			static_assert(sizeof(wchar_t) == sizeof(char32));

			const size_t requiredLength = simdutf::utf32_length_from_utf8(s.data(), s.size());

			std::wstring result;

			result.resize_and_overwrite(requiredLength, [&](wchar_t* buf, size_t) {
				return simdutf::convert_utf8_to_utf32(s.data(), s.size(), static_cast<char32*>(static_cast<void*>(buf)));
				});

			return result;

		# endif
		}

		std::wstring ToWstring(const StringView s)
		{
		# if SIV3D_PLATFORM(WINDOWS)

			static_assert(sizeof(wchar_t) == sizeof(char16));
			
			const size_t requiredLength = simdutf::utf16_length_from_utf32(s.data(), s.size());

			std::wstring result;

			result.resize_and_overwrite(requiredLength, [&](wchar_t* buf, size_t) {
				return simdutf::convert_utf32_to_utf16le(s.data(), s.size(), static_cast<char16*>(static_cast<void*>(buf)));
				});

			return result;

		# else

			static_assert(sizeof(wchar_t) == sizeof(char32));

			return std::wstring(s.begin(), s.end());

		# endif
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

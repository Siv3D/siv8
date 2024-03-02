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

# include <Siv3D/Unicode.hpp>
# include <Siv3D/String.hpp>
# include <ThirdParty/simdutf/simdutf.h>
# include "../UnicodeConverter/UnicodeUtility.hpp"

namespace s3d
{
	namespace Unicode
	{			
		////////////////////////////////////////////////////////////////
		//
		//	FromAscii
		//
		////////////////////////////////////////////////////////////////

		String FromAscii(const std::string_view asciiText)
		{
			const size_t requiredLength = asciiText.size();
			
			String result;

			if (requiredLength < 32) // 短い文字列は非 SIMD
			{
				result.resize_and_overwrite(asciiText.size(), [&](char32* buf, size_t n) -> size_t
					{
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
			}
			else
			{
				if (not simdutf::validate_ascii(asciiText.data(), asciiText.size()))
				{
					return result;
				}

				result.resize_and_overwrite(requiredLength, [&](char32* buf, size_t)
					{
						return simdutf::convert_utf8_to_utf32(asciiText.data(), asciiText.size(), buf);
					});
			}

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	FromUTF8
		//
		////////////////////////////////////////////////////////////////

		String FromUTF8(const std::string_view s)
		{
			const size_t requiredLength = simdutf::utf32_length_from_utf8(s.data(), s.size());

			String result;
			
			result.resize_and_overwrite(requiredLength, [&](char32* buf, size_t)
				{
					return simdutf::convert_utf8_to_utf32(s.data(), s.size(), buf);
				});

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	FromUTF16
		//
		////////////////////////////////////////////////////////////////

		String FromUTF16(const std::u16string_view s)
		{
			const size_t requiredLength = simdutf::utf32_length_from_utf16le(s.data(), s.size());

			String result;
			
			result.resize_and_overwrite(requiredLength, [&](char32* buf, size_t)
				{
					return simdutf::convert_utf16le_to_utf32(s.data(), s.size(), buf);
				});

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	FromWstring
		//
		////////////////////////////////////////////////////////////////

		String FromWstring(const std::wstring_view s)
		{
		# if SIV3D_PLATFORM(WINDOWS)

			static_assert(sizeof(wchar_t) == 2);

			const char16* pSrc = static_cast<const char16*>(static_cast<const void*>(s.data()));

			return FromUTF16(std::u16string_view{ pSrc, s.size() });

		# else

			static_assert(sizeof(wchar_t) == 4);

			return String(s.begin(), s.end());

		# endif
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	FromUTF32
		//
		////////////////////////////////////////////////////////////////

		String FromUTF32(const std::u32string_view s)
		{
			return String{ s };
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToAscii
		//
		////////////////////////////////////////////////////////////////

		std::string ToAscii(const StringView asciiText)
		{
			std::string result;

			result.resize_and_overwrite(asciiText.size(), [&](char* buf, size_t n) -> size_t
				{
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

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToUTF8
		//
		////////////////////////////////////////////////////////////////

		std::string ToUTF8(const StringView s)
		{
		// macOS で simdutf を使うと時々謎のクラッシュが起こるため回避
		# if SIV3D_PLATFORM(MACOS)
			
			std::string result(detail::UTF8_Length(s), '0');

			const char32* pSrc = s.data();
			const char32* const pSrcEnd = pSrc + s.size();
			char8* pDst = &result[0];

			while (pSrc != pSrcEnd)
			{
				detail::UTF8_Encode(&pDst, *pSrc++);
			}

			return result;
			
		# else
			
			const size_t requiredLength = simdutf::utf8_length_from_utf32(s.data(), s.size());
			
			std::string result;

			result.resize_and_overwrite(requiredLength, [&](char* buf, size_t)
				{
					return simdutf::convert_utf32_to_utf8(s.data(), s.size(), buf);
				});

			return result;
		
		# endif
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToUTF16
		//
		////////////////////////////////////////////////////////////////

		std::u16string ToUTF16(const StringView s)
		{
			const size_t requiredLength = simdutf::utf16_length_from_utf32(s.data(), s.size());

			std::u16string result;

			result.resize_and_overwrite(requiredLength, [&](char16* buf, size_t)
				{
					return simdutf::convert_utf32_to_utf16le(s.data(), s.size(), buf);
				});

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToWstring
		//
		////////////////////////////////////////////////////////////////

		std::wstring ToWstring(std::string_view s)
		{
		# if SIV3D_PLATFORM(WINDOWS)

			static_assert(sizeof(wchar_t) == sizeof(char16));
			
			const size_t requiredLength = simdutf::utf16_length_from_utf8(s.data(), s.size());

			std::wstring result;

			result.resize_and_overwrite(requiredLength, [&](wchar_t* buf, size_t)
				{
					return simdutf::convert_utf8_to_utf16le(s.data(), s.size(), static_cast<char16*>(static_cast<void*>(buf)));
				});

			return result;

		# else

			static_assert(sizeof(wchar_t) == sizeof(char32));

			const size_t requiredLength = simdutf::utf32_length_from_utf8(s.data(), s.size());

			std::wstring result;

			result.resize_and_overwrite(requiredLength, [&](wchar_t* buf, size_t)
				{
					return simdutf::convert_utf8_to_utf32(s.data(), s.size(), static_cast<char32*>(static_cast<void*>(buf)));
				});

			return result;

		# endif
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToWstring
		//
		////////////////////////////////////////////////////////////////

		std::wstring ToWstring(const StringView s)
		{
		# if SIV3D_PLATFORM(WINDOWS)

			static_assert(sizeof(wchar_t) == sizeof(char16));
			
			const size_t requiredLength = simdutf::utf16_length_from_utf32(s.data(), s.size());

			std::wstring result;

			result.resize_and_overwrite(requiredLength, [&](wchar_t* buf, size_t)
				{
					return simdutf::convert_utf32_to_utf16le(s.data(), s.size(), static_cast<char16*>(static_cast<void*>(buf)));
				});

			return result;

		# else

			static_assert(sizeof(wchar_t) == sizeof(char32));

			return std::wstring(s.begin(), s.end());

		# endif
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToUTF32
		//
		////////////////////////////////////////////////////////////////

		std::u32string ToUTF32(const StringView s)
		{
			return std::u32string(s.begin(), s.end());
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	UTF8ToUTF16, UTF8ToUTF32
		//
		////////////////////////////////////////////////////////////////

		std::u16string UTF8ToUTF16(const std::string_view s)
		{
			const size_t requiredLength = simdutf::utf16_length_from_utf8(s.data(), s.size());

			std::u16string result;

			result.resize_and_overwrite(requiredLength, [&](char16* buf, size_t)
				{
					return simdutf::convert_utf8_to_utf16le(s.data(), s.size(), buf);
				});

			return result;
		}

		std::u32string UTF8ToUTF32(const std::string_view s)
		{
			const size_t requiredLength = simdutf::utf32_length_from_utf8(s.data(), s.size());

			std::u32string result;

			result.resize_and_overwrite(requiredLength, [&](char32* buf, size_t)
				{
					return simdutf::convert_utf8_to_utf32(s.data(), s.size(), buf);
				});

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	UTF16ToUTF8, UTF16ToUTF32
		//
		////////////////////////////////////////////////////////////////

		std::string UTF16ToUTF8(const std::u16string_view s)
		{
			const size_t requiredLength = simdutf::utf8_length_from_utf16le(s.data(), s.size());

			std::string result;

			result.resize_and_overwrite(requiredLength, [&](char* buf, size_t)
				{
					return simdutf::convert_utf16le_to_utf8(s.data(), s.size(), buf);
				});

			return result;
		}

		std::u32string UTF16ToUTF32(const std::u16string_view s)
		{
			const size_t requiredLength = simdutf::utf32_length_from_utf16le(s.data(), s.size());

			std::u32string result;

			result.resize_and_overwrite(requiredLength, [&](char32* buf, size_t)
				{
					return simdutf::convert_utf16le_to_utf32(s.data(), s.size(), buf);
				});

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	UTF32ToUTF8, UTF32ToUTF16
		//
		////////////////////////////////////////////////////////////////

		std::string UTF32ToUTF8(const std::u32string_view s)
		{
			const size_t requiredLength = simdutf::utf8_length_from_utf32(s.data(), s.size());

			std::string result;

			result.resize_and_overwrite(requiredLength, [&](char* buf, size_t)
				{
					return simdutf::convert_utf32_to_utf8(s.data(), s.size(), buf);
				});

			return result;
		}

		std::u16string UTF32ToUTF16(const std::u32string_view s)
		{
			const size_t requiredLength = simdutf::utf16_length_from_utf32(s.data(), s.size());

			std::u16string result;

			result.resize_and_overwrite(requiredLength, [&](char16* buf, size_t)
				{
					return simdutf::convert_utf32_to_utf16le(s.data(), s.size(), buf);
				});

			return result;
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ValidateAscii
		//
		////////////////////////////////////////////////////////////////

		Result<void, size_t> ValidateAscii(const std::string_view s)
		{
			const simdutf::result result = simdutf::validate_ascii_with_errors(s.data(), s.size());

			if (result.error == simdutf::SUCCESS)
			{
				return {};
			}
			else
			{
				return Err{ result.count };
			}
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	VaidateUTF8
		//
		////////////////////////////////////////////////////////////////

		Result<void, size_t> ValidateUTF8(const std::string_view s)
		{
			const simdutf::result result = simdutf::validate_utf8_with_errors(s.data(), s.size());

			if (result.error == simdutf::SUCCESS)
			{
				return {};
			}
			else
			{
				return Err{ result.count };
			}
		}
	}
}

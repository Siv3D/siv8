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

# include <Siv3D/Unicode.hpp>
# include <Siv3D/String.hpp>
# include <ThirdParty/simdutf/simdutf.h>
# include <type_traits>

namespace s3d
{
	namespace Unicode
	{
		namespace
		{
			template <class Destination>
			void ConvertUTF8ToUTF32(const std::string_view s, Destination& dst)
			{
				const size_t requiredLength = simdutf::utf32_length_from_utf8(s.data(), s.size());
				dst.resize_and_overwrite(requiredLength, [&](char32* buf, size_t)
					{
						return simdutf::convert_utf8_to_utf32(s.data(), s.size(), buf);
					});
			}

			template <class Destination>
			void ConvertUTF16ToUTF32(const std::u16string_view s, Destination& dst)
			{
				const size_t requiredLength = simdutf::utf32_length_from_utf16le(s.data(), s.size());
				dst.resize_and_overwrite(requiredLength, [&](char32* buf, size_t)
					{
						return simdutf::convert_utf16le_to_utf32(s.data(), s.size(), buf);
					});
			}
		# if not SIV3D_PLATFORM(WINDOWS)

			template <class View, class Destination>
			void CopyValidatedUTF32(const View s, Destination& dst)
			{
				static_assert(sizeof(typename Destination::value_type) == sizeof(char32));
				static_assert(sizeof(typename View::value_type) == sizeof(char32));

				if (s.size() < 16)
				{
					dst.resize_and_overwrite(s.size(), [&](auto* buf, size_t) -> size_t
						{
							for (size_t i = 0; i < s.size(); ++i)
							{
								const uint32 cp = static_cast<uint32>(s[i]);
								if ((0x10FFFFu < cp) || ((0xD800u <= cp) && (cp <= 0xDFFFu)))
								{
									return 0;
								}
								buf[i] = static_cast<typename Destination::value_type>(cp);
							}
							return s.size();
						});
					return;
				}

				if constexpr (std::is_same_v<typename Destination::value_type, char32>)
				{
					dst.assign(s.begin(), s.end());
					if (not simdutf::validate_utf32(dst.data(), dst.size()))
					{
						dst.clear();
					}
				}
				else
				{
					if (not simdutf::validate_utf32(s.data(), s.size()))
					{
						dst.clear();
						return;
					}
					dst.assign(s.begin(), s.end());
				}
			}

		# endif
		}

		////////////////////////////////////////////////////////////////
		//
		//	FromAscii
		//
		////////////////////////////////////////////////////////////////

		String FromAscii(const std::string_view asciiText)
		{
			String result;
			FromAscii(asciiText, result);
			return result;
		}

		void FromAscii(const std::string_view asciiText, String& dst)
		{
			const size_t requiredLength = asciiText.size();

			if (requiredLength < 32) // 短い文字列は非 SIMD
			{
				dst.resize_and_overwrite(asciiText.size(), [&](char32* buf, size_t n) -> size_t
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
					dst.clear();
					return;
				}

				dst.resize_and_overwrite(requiredLength, [&](char32* buf, size_t)
					{
						return simdutf::convert_latin1_to_utf32(asciiText.data(), asciiText.size(), buf);
					});
			}
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	FromUTF8
		//
		////////////////////////////////////////////////////////////////

		String FromUTF8(const std::string_view s)
		{
			String result;
			FromUTF8(s, result);
			return result;
		}

		void FromUTF8(const std::string_view s, String& dst)
		{
			ConvertUTF8ToUTF32(s, dst);
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	FromUTF16
		//
		////////////////////////////////////////////////////////////////

		String FromUTF16(const std::u16string_view s)
		{
			String result;
			FromUTF16(s, result);
			return result;
		}

		void FromUTF16(const std::u16string_view s, String& dst)
		{
			ConvertUTF16ToUTF32(s, dst);
		}

		////////////////////////////////////////////////////////////////
		//
		//	FromUTF16BE
		//
		////////////////////////////////////////////////////////////////

		String FromUTF16BE(std::u16string_view s)
		{
			String result;
			FromUTF16BE(s, result);
			return result;
		}

		void FromUTF16BE(std::u16string_view s, String& dst)
		{
			const size_t requiredLength = simdutf::utf32_length_from_utf16be(s.data(), s.size());

			dst.resize_and_overwrite(requiredLength, [&](char32* buf, size_t)
				{
					return simdutf::convert_utf16be_to_utf32(s.data(), s.size(), buf);
				});
		}

		////////////////////////////////////////////////////////////////
		//
		//	FromWstring
		//
		////////////////////////////////////////////////////////////////

		String FromWstring(const std::wstring_view s)
		{
			String result;
			FromWstring(s, result);
			return result;
		}

		void FromWstring(const std::wstring_view s, String& dst)
		{
		# if SIV3D_PLATFORM(WINDOWS)

			static_assert(sizeof(wchar_t) == 2);

			const char16* pSrc = static_cast<const char16*>(static_cast<const void*>(s.data()));

			FromUTF16(std::u16string_view{ pSrc, s.size() }, dst);

		# else

			static_assert(sizeof(wchar_t) == 4);

			CopyValidatedUTF32(s, dst);

		# endif
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	FromUTF32
		//
		////////////////////////////////////////////////////////////////

		String FromUTF32(const std::u32string_view s)
		{
			String result;
			FromUTF32(s, result);
			return result;
		}

		void FromUTF32(const std::u32string_view s, String& dst)
		{
			dst.assign(s);
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToAscii
		//
		////////////////////////////////////////////////////////////////

		std::string ToAscii(const StringView asciiText)
		{
			std::string result;
			ToAscii(asciiText, result);
			return result;
		}

		void ToAscii(const StringView asciiText, std::string& dst)
		{
			dst.resize_and_overwrite(asciiText.size(), [&](char* buf, size_t n) -> size_t
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
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToUTF8
		//
		////////////////////////////////////////////////////////////////

		std::string ToUTF8(const StringView s)
		{
			std::string result;
			ToUTF8(s, result);
			return result;
		}

		void ToUTF8(const StringView s, std::string& dst)
		{
			UTF32ToUTF8(static_cast<std::u32string_view>(s), dst);
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToUTF16
		//
		////////////////////////////////////////////////////////////////

		std::u16string ToUTF16(const StringView s)
		{
			std::u16string result;
			ToUTF16(s, result);
			return result;
		}

		void ToUTF16(const StringView s, std::u16string& dst)
		{
			UTF32ToUTF16(static_cast<std::u32string_view>(s), dst);
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToWstring
		//
		////////////////////////////////////////////////////////////////

		std::wstring ToWstring(std::string_view s)
		{
			std::wstring result;
			ToWstring(s, result);
			return result;
		}

		void ToWstring(std::string_view s, std::wstring& dst)
		{
		# if SIV3D_PLATFORM(WINDOWS)

			static_assert(sizeof(wchar_t) == sizeof(char16));
			
			const size_t requiredLength = simdutf::utf16_length_from_utf8(s.data(), s.size());

			dst.resize_and_overwrite(requiredLength, [&](wchar_t* buf, size_t)
				{
					return simdutf::convert_utf8_to_utf16le(s.data(), s.size(), static_cast<char16*>(static_cast<void*>(buf)));
				});

		# else

			static_assert(sizeof(wchar_t) == sizeof(char32));

			const size_t requiredLength = simdutf::utf32_length_from_utf8(s.data(), s.size());

			dst.resize_and_overwrite(requiredLength, [&](wchar_t* buf, size_t)
				{
					return simdutf::convert_utf8_to_utf32(s.data(), s.size(), static_cast<char32*>(static_cast<void*>(buf)));
				});

		# endif
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToWstring
		//
		////////////////////////////////////////////////////////////////

		std::wstring ToWstring(const StringView s)
		{
			std::wstring result;
			ToWstring(s, result);
			return result;
		}

		void ToWstring(const StringView s, std::wstring& dst)
		{
		# if SIV3D_PLATFORM(WINDOWS)

			static_assert(sizeof(wchar_t) == sizeof(char16));
			
			const size_t requiredLength = simdutf::utf16_length_from_utf32(s.data(), s.size());

			dst.resize_and_overwrite(requiredLength, [&](wchar_t* buf, size_t)
				{
					return simdutf::convert_utf32_to_utf16le(s.data(), s.size(), static_cast<char16*>(static_cast<void*>(buf)));
				});

		# else

			static_assert(sizeof(wchar_t) == sizeof(char32));

			CopyValidatedUTF32(s, dst);

		# endif
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ToUTF32
		//
		////////////////////////////////////////////////////////////////

		std::u32string ToUTF32(const StringView s)
		{
			std::u32string result;
			ToUTF32(s, result);
			return result;
		}

		void ToUTF32(const StringView s, std::u32string& dst)
		{
			dst.assign(static_cast<std::u32string_view>(s));
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	UTF8ToUTF16, UTF8ToUTF32
		//
		////////////////////////////////////////////////////////////////

		std::u16string UTF8ToUTF16(const std::string_view s)
		{
			std::u16string result;
			UTF8ToUTF16(s, result);
			return result;
		}

		void UTF8ToUTF16(const std::string_view s, std::u16string& dst)
		{
			const size_t requiredLength = simdutf::utf16_length_from_utf8(s.data(), s.size());

			dst.resize_and_overwrite(requiredLength, [&](char16* buf, size_t)
				{
					return simdutf::convert_utf8_to_utf16le(s.data(), s.size(), buf);
				});
		}

		std::u32string UTF8ToUTF32(const std::string_view s)
		{
			std::u32string result;
			UTF8ToUTF32(s, result);
			return result;
		}

		void UTF8ToUTF32(const std::string_view s, std::u32string& dst)
		{
			ConvertUTF8ToUTF32(s, dst);
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	UTF16ToUTF8, UTF16ToUTF32
		//
		////////////////////////////////////////////////////////////////

		std::string UTF16ToUTF8(const std::u16string_view s)
		{
			std::string result;
			UTF16ToUTF8(s, result);
			return result;
		}

		void UTF16ToUTF8(const std::u16string_view s, std::string& dst)
		{
			const size_t requiredLength = simdutf::utf8_length_from_utf16le(s.data(), s.size());

			dst.resize_and_overwrite(requiredLength, [&](char* buf, size_t)
				{
					return simdutf::convert_utf16le_to_utf8(s.data(), s.size(), buf);
				});
		}

		std::u32string UTF16ToUTF32(const std::u16string_view s)
		{
			std::u32string result;
			UTF16ToUTF32(s, result);
			return result;
		}

		void UTF16ToUTF32(const std::u16string_view s, std::u32string& dst)
		{
			ConvertUTF16ToUTF32(s, dst);
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	UTF32ToUTF8, UTF32ToUTF16
		//
		////////////////////////////////////////////////////////////////

		std::string UTF32ToUTF8(const std::u32string_view s)
		{
			std::string result;
			UTF32ToUTF8(s, result);
			return result;
		}

		void UTF32ToUTF8(const std::u32string_view s, std::string& dst)
		{
			const size_t requiredLength = simdutf::utf8_length_from_utf32(s.data(), s.size());

			dst.resize_and_overwrite(requiredLength, [&](char* buf, size_t)
				{
					return simdutf::convert_utf32_to_utf8(s.data(), s.size(), buf);
				});
		}

		std::u16string UTF32ToUTF16(const std::u32string_view s)
		{
			std::u16string result;
			UTF32ToUTF16(s, result);
			return result;
		}

		void UTF32ToUTF16(const std::u32string_view s, std::u16string& dst)
		{
			const size_t requiredLength = simdutf::utf16_length_from_utf32(s.data(), s.size());

			dst.resize_and_overwrite(requiredLength, [&](char16* buf, size_t)
				{
					return simdutf::convert_utf32_to_utf16le(s.data(), s.size(), buf);
				});
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
				return{};
			}
			else
			{
				return Err{ result.count };
			}
		}
			
		////////////////////////////////////////////////////////////////
		//
		//	ValidateUTF8
		//
		////////////////////////////////////////////////////////////////

		Result<void, size_t> ValidateUTF8(const std::string_view s)
		{
			const simdutf::result result = simdutf::validate_utf8_with_errors(s.data(), s.size());

			if (result.error == simdutf::SUCCESS)
			{
				return{};
			}
			else
			{
				return Err{ result.count };
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	ValidateUTF16
		//
		////////////////////////////////////////////////////////////////

		Result<void, size_t> ValidateUTF16(const std::u16string_view s)
		{
			const simdutf::result result = simdutf::validate_utf16le_with_errors(s.data(), s.size());
			
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
		//	ValidateUTF16BE
		//
		////////////////////////////////////////////////////////////////

		Result<void, size_t> ValidateUTF16BE(const std::u16string_view s)
		{
			const simdutf::result result = simdutf::validate_utf16be_with_errors(s.data(), s.size());

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
		//	ValidateUTF32
		//
		////////////////////////////////////////////////////////////////

		Result<void, size_t> ValidateUTF32(const std::u32string_view s)
		{
			const simdutf::result result = simdutf::validate_utf32_with_errors(s.data(), s.size());

			if (result.error == simdutf::SUCCESS)
			{
				return{};
			}
			else
			{
				return Err{ result.count };
			}
		}
	}
}

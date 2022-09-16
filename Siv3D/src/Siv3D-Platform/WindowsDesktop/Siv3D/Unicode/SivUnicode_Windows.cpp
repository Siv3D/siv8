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

# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Unicode.hpp>
# include <ThirdParty/simdutf/simdutf.h>

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		static std::string ToMultiByte(const std::wstring_view s, const uint32 codePage)
		{
			if (s.empty())
			{
				return{};
			}

			const int32 requiredLength = ::WideCharToMultiByte(codePage, 0,
				s.data(), static_cast<int>(s.length()),
				nullptr, 0, nullptr, nullptr);

			std::string result;

			result.resize_and_overwrite(requiredLength, [&](char* buf, size_t) {
				return ::WideCharToMultiByte(codePage, 0,
					s.data(), static_cast<int>(s.length()),
					buf, requiredLength, nullptr, nullptr);
				});

			return result;
		}

		[[nodiscard]]
		static std::u16string FromMultiByte(const std::string_view s, const uint32 codePage)
		{
			if (s.empty())
			{
				return{};
			}

			const int32 requiredLength = ::MultiByteToWideChar(codePage, 0,
				s.data(), static_cast<int32>(s.length()),
				nullptr, 0);

			if (requiredLength == 0)
			{
				return{};
			}

			std::u16string result;

			result.resize_and_overwrite(requiredLength, [&](char16* buf, size_t) {
				return ::MultiByteToWideChar(codePage, 0,
					s.data(), static_cast<int32>(s.length()),
					static_cast<wchar_t*>(static_cast<void*>(buf)), requiredLength);
				});

			return result;
		}
	}

	namespace Unicode
	{
		String Widen(const std::string_view s)
		{
			return FromUTF16(detail::FromMultiByte(s, CP_ACP));
		}

		String FromWstring(const std::wstring_view s)
		{
			const char16* pSrc = static_cast<const char16*>(static_cast<const void*>(s.data()));
			
			return FromUTF16(std::u16string_view(pSrc, s.size()));
		}

		std::string Narrow(const StringView s)
		{
			return detail::ToMultiByte(ToWstring(s), CP_ACP);
		}

		std::wstring ToWstring(const StringView s)
		{
			const size_t requiredLength = simdutf::utf16_length_from_utf32(s.data(), s.size());

			std::wstring result;

			result.resize_and_overwrite(requiredLength, [&](wchar_t* buf, size_t) {
				return simdutf::convert_utf32_to_utf16le(s.data(), s.size(), static_cast<char16*>(static_cast<void*>(buf)));
				});

			return result;
		}
	}
}

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

			std::string result(requiredLength, '\0');

			::WideCharToMultiByte(codePage, 0,
				s.data(), static_cast<int>(s.length()),
				result.data(), requiredLength, nullptr, nullptr);

			return result;
		}
	}

	namespace Unicode
	{
		std::string Narrow(const StringView s)
		{
			return detail::ToMultiByte(ToWstring(s), CP_ACP);
		}

		std::wstring ToWstring(const StringView s)
		{
			const size_t utf16Length = simdutf::utf16_length_from_utf32(s.data(), s.size());

			std::wstring result(utf16Length, L'\0');

			simdutf::convert_utf32_to_utf16le(s.data(), s.size(), static_cast<char16*>(static_cast<void*>(result.data())));

			return result;
		}
	}
}

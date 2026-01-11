//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/PercentEncode.hpp>
# include <Siv3D/Char.hpp>
# include <Siv3D/Byte.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	PercentEncode
	//
	////////////////////////////////////////////////////////////////

	std::string PercentEncode(const std::string_view s)
	{
		std::string result;
		result.reserve(s.size());

		for (const auto ch : s)
		{
			if (IsAlnum(ch) || (ch == '-') || (ch == '_') || (ch == '.') || (ch == '~'))
			{
				result.push_back(ch);
			}
			else
			{
				const std::array<char8, 2> hex = ToHexUpperCase(static_cast<Byte>(ch));
				result.push_back('%');
				result.push_back(hex[0]);
				result.push_back(hex[1]);
			}
		}

		return result;
	}

	String PercentEncode(const StringView s)
	{
		String result;
		result.reserve(s.size());

		for (const auto ch : s.toUTF8())
		{
			if (IsAlnum(ch) || (ch == '-') || (ch == '_') || (ch == '.') || (ch == '~'))
			{
				result.push_back(ch);
			}
			else
			{
				const std::array<char8, 2> hex = ToHexUpperCase(static_cast<Byte>(ch));
				result.push_back('%');
				result.push_back(hex[0]);
				result.push_back(hex[1]);
			}
		}

		return result;
	}
}

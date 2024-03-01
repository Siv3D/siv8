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

# include <Siv3D/EscapedFormatter.hpp>

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		static constexpr char32 ToHexChar(const uint32 value) noexcept
		{
			return U"0123456789abcdef"[value & 0xF];
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	EscapeFormatter
	//
	////////////////////////////////////////////////////////////////

	void EscapedFormatter(FormatData& formatData, const char ch)
	{
		EscapedFormatter(formatData, static_cast<char32_t>(ch));
	}

	void EscapedFormatter(FormatData& formatData, const char8_t ch)
	{
		EscapedFormatter(formatData, static_cast<char32_t>(ch));
	}

	void EscapedFormatter(FormatData& formatData, const char16_t ch)
	{
		EscapedFormatter(formatData, static_cast<char32_t>(ch));
	}

	void EscapedFormatter(FormatData& formatData, const wchar_t ch)
	{
		EscapedFormatter(formatData, static_cast<char32_t>(ch));
	}

	void EscapedFormatter(FormatData& formatData, const char32_t ch)
	{
		formatData.string.push_back(U'\'');

		if (ch == U'\t')
		{
			formatData.string.append(U"\\t"_sv);
		}
		else if (ch == U'\n')
		{
			formatData.string.append(U"\\n"_sv);
		}
		else if (ch == U'\r')
		{
			formatData.string.append(U"\\r"_sv);
		}
		else if (ch == U'\'')
		{
			formatData.string.append(U"\\\'"_sv);
		}
		else if (ch == U'\\')
		{
			formatData.string.append(U"\\\\"_sv);
		}
		else if (ch < 0x10)
		{
			formatData.string.append(U"\\u{"_sv);
			formatData.string.push_back(detail::ToHexChar(ch));
			formatData.string.push_back(U'}');
		}
		else if (ch < 0x20)
		{
			formatData.string.append(U"\\u{"_sv);
			formatData.string.push_back(detail::ToHexChar(ch >> 4));
			formatData.string.push_back(detail::ToHexChar(ch));
			formatData.string.push_back(U'}');
		}
		else if ((0x7F <= ch) && (ch <= 0xFF))
		{
			formatData.string.append(U"\\x{"_sv);
			formatData.string.push_back(detail::ToHexChar(ch >> 4));
			formatData.string.push_back(detail::ToHexChar(ch));
			formatData.string.push_back(U'}');
		}
		else
		{
			formatData.string.push_back(ch);
		}

		formatData.string.push_back(U'\'');
	}

	void EscapedFormatter(FormatData& formatData, const char32_t* s)
	{
		EscapedFormatter(formatData, StringView{ s });
	}

	void EscapedFormatter(FormatData& formatData, const std::u32string& s)
	{
		EscapedFormatter(formatData, StringView{ s });
	}

	void EscapedFormatter(FormatData& formatData, const String& s)
	{
		EscapedFormatter(formatData, StringView{ s });
	}

	void EscapedFormatter(FormatData& formatData, const StringView s)
	{
		formatData.string.push_back(U'\"');

		for (const auto ch : s)
		{
			if (ch == U'\t')
			{
				formatData.string.append(U"\\t"_sv);
			}
			else if (ch == U'\n')
			{
				formatData.string.append(U"\\n"_sv);
			}
			else if (ch == U'\r')
			{
				formatData.string.append(U"\\r"_sv);
			}
			else if (ch == U'\"')
			{
				formatData.string.append(U"\\\""_sv);
			}
			else if (ch == U'\\')
			{
				formatData.string.append(U"\\\\"_sv);
			}
			else if (ch < 0x10)
			{
				formatData.string.append(U"\\u{"_sv);
				formatData.string.push_back(detail::ToHexChar(ch));
				formatData.string.push_back(U'}');
			}
			else if (ch < 0x20)
			{
				formatData.string.append(U"\\u{"_sv);
				formatData.string.push_back(detail::ToHexChar(ch >> 4));
				formatData.string.push_back(detail::ToHexChar(ch));
				formatData.string.push_back(U'}');
			}
			else if ((0x7F <= ch) && (ch <= 0xFF))
			{
				formatData.string.append(U"\\x{"_sv);
				formatData.string.push_back(detail::ToHexChar(ch >> 4));
				formatData.string.push_back(detail::ToHexChar(ch));
				formatData.string.push_back(U'}');
			}
			else
			{
				formatData.string.push_back(ch);
			}
		}

		formatData.string.push_back(U'\"');
	}
}

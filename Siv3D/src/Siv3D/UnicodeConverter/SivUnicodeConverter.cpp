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

# include <Siv3D/UnicodeConverter.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	put
	//
	////////////////////////////////////////////////////////////////

	UTF8toUTF32_Converter::DecodeResult UTF8toUTF32_Converter::put(const char8 code) noexcept
	{
		const uint8 byte = static_cast<uint8>(code);

		if (m_remaining == 0)
		{
			if (byte < 0x80)
			{
				m_result = byte;
				return { UnicodeDecodeStatus::Ready, true };
			}

			m_lower = 0x80;
			m_upper = 0xBF;

			if ((0xC2 <= byte) && (byte <= 0xDF))
			{
				m_codePoint = (byte & 0x1F);
				m_remaining = 1;
			}
			else if ((0xE0 <= byte) && (byte <= 0xEF))
			{
				m_codePoint = (byte & 0x0F);
				m_remaining = 2;
				// Exclude overlong encodings and surrogate values at the second byte.
				m_lower = ((byte == 0xE0) ? 0xA0 : 0x80);
				m_upper = ((byte == 0xED) ? 0x9F : 0xBF);
			}
			else if ((0xF0 <= byte) && (byte <= 0xF4))
			{
				m_codePoint = (byte & 0x07);
				m_remaining = 3;
				// Exclude overlong encodings and values above U+10FFFF.
				m_lower = ((byte == 0xF0) ? 0x90 : 0x80);
				m_upper = ((byte == 0xF4) ? 0x8F : 0xBF);
			}
			else
			{
				m_result = 0xFFFD;
				return { UnicodeDecodeStatus::Invalid, true };
			}

			return { UnicodeDecodeStatus::NeedMore, true };
		}

		if ((byte < m_lower) || (m_upper < byte))
		{
			// End the maximal subpart before this byte; the caller must retry it.
			m_remaining = 0;
			m_result = 0xFFFD;
			return { UnicodeDecodeStatus::Invalid, false };
		}

		m_codePoint = ((m_codePoint << 6) | (byte & 0x3F));
		m_lower = 0x80;
		m_upper = 0xBF;

		if (--m_remaining)
		{
			return { UnicodeDecodeStatus::NeedMore, true };
		}

		m_result = m_codePoint;
		return { UnicodeDecodeStatus::Ready, true };
	}

	UTF16toUTF32_Converter::DecodeResult UTF16toUTF32_Converter::put(const char16 code) noexcept
	{
		if (m_highSurrogate)
		{
			const char16 high = m_highSurrogate;
			m_highSurrogate = 0;

			if ((0xDC00 <= code) && (code <= 0xDFFF))
			{
				m_result = (0x10000 + ((high - 0xD800) << 10) + (code - 0xDC00));
				return { UnicodeDecodeStatus::Ready, true };
			}

			m_result = 0xFFFD;
			return { UnicodeDecodeStatus::Invalid, false };
		}

		if ((0xD800 <= code) && (code <= 0xDBFF))
		{
			m_highSurrogate = code;
			return { UnicodeDecodeStatus::NeedMore, true };
		}

		if ((0xDC00 <= code) && (code <= 0xDFFF))
		{
			m_result = 0xFFFD;
			return { UnicodeDecodeStatus::Invalid, true };
		}

		m_result = code;
		return { UnicodeDecodeStatus::Ready, true };
	}

	size_t UTF32toUTF8_Converter::put(const char32 code) noexcept
	{
		if (code < 0x80)
		{
			m_buffer[0] = static_cast<char8>(code);
			return 1;
		}

		if (code < 0x800)
		{
			m_buffer[0] = static_cast<char8>(0xC0 | (code >> 6));
			m_buffer[1] = static_cast<char8>(0x80 | (code & 0x3F));
			return 2;
		}

		if (((0xD800 <= code) && (code <= 0xDFFF)) || (0x10FFFF < code))
		{
			m_buffer[0] = static_cast<char8>(0xEF);
			m_buffer[1] = static_cast<char8>(0xBF);
			m_buffer[2] = static_cast<char8>(0xBD);
			return 3;
		}

		if (code < 0x10000)
		{
			m_buffer[0] = static_cast<char8>(0xE0 | (code >> 12));
			m_buffer[1] = static_cast<char8>(0x80 | ((code >> 6) & 0x3F));
			m_buffer[2] = static_cast<char8>(0x80 | (code & 0x3F));
			return 3;
		}

		m_buffer[0] = static_cast<char8>(0xF0 | (code >> 18));
		m_buffer[1] = static_cast<char8>(0x80 | ((code >> 12) & 0x3F));
		m_buffer[2] = static_cast<char8>(0x80 | ((code >> 6) & 0x3F));
		m_buffer[3] = static_cast<char8>(0x80 | (code & 0x3F));
		return 4;
	}

	size_t UTF32toUTF16_Converter::put(const char32 code) noexcept
	{
		if (code < 0xD800)
		{
			m_buffer[0] = static_cast<char16>(code);
			return 1;
		}

		if ((code <= 0xDFFF) || (0x10FFFF < code))
		{
			m_buffer[0] = 0xFFFD;
			return 1;
		}

		if (code < 0x10000)
		{
			m_buffer[0] = static_cast<char16>(code);
			return 1;
		}

		m_buffer[0] = static_cast<char16>(0xD800 + ((code - 0x10000) >> 10));
		m_buffer[1] = static_cast<char16>(0xDC00 + ((code - 0x10000) & 0x3FF));
		return 2;
	}

	////////////////////////////////////////////////////////////////
	//
	//	finish
	//
	////////////////////////////////////////////////////////////////

	bool UTF8toUTF32_Converter::finish() noexcept
	{
		if (m_remaining == 0)
		{
			return false;
		}

		m_remaining = 0;
		m_result = 0xFFFD;
		return true;
	}

	bool UTF16toUTF32_Converter::finish() noexcept
	{
		if (m_highSurrogate == 0)
		{
			return false;
		}

		m_highSurrogate = 0;
		m_result = 0xFFFD;
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	void UTF8toUTF32_Converter::reset() noexcept
	{
		*this = {};
	}

	void UTF16toUTF32_Converter::reset() noexcept
	{
		*this = {};
	}
}

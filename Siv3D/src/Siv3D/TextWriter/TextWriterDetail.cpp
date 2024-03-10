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

# include "TextWriterDetail.hpp"
# include <Siv3D/Endian.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/UnicodeConverter.hpp>

namespace s3d
{
	TextWriter::TextWriterDetail::~TextWriterDetail()
	{
		close();
	}

	bool TextWriter::TextWriterDetail::open(const FilePathView path, const OpenMode openMode, const TextEncoding encoding)
	{
		close();

		if (openMode == OpenMode::Append)
		{
			m_encoding = Unicode::GetTextEncoding(path);
		}
		else
		{
			m_encoding = encoding;
		}

		if (not m_binaryWriter.open(path, openMode))
		{
			return false;
		}

		if (0 < m_binaryWriter.size())
		{
			return true;
		}

		switch (m_encoding)
		{
		case TextEncoding::UTF8_WITH_BOM:
			{
				m_binaryWriter.write(BOM_UTF8);
				break;
			}
		case TextEncoding::UTF16LE:
			{
				m_binaryWriter.write(BOM_UTF16LE);
				break;
			}
		case TextEncoding::UTF16BE:
			{
				m_binaryWriter.write(BOM_UTF16BE);
				break;
			}
		default:
			break;
		}

		return true;
	}

	void TextWriter::TextWriterDetail::close()
	{
		m_binaryWriter.close();

		m_encoding = TextEncoding::Default;
	}

	bool TextWriter::TextWriterDetail::isOpen() const noexcept
	{
		return m_binaryWriter.isOpen();
	}

	void TextWriter::TextWriterDetail::clear()
	{
		if (not isOpen())
		{
			return;
		}

		const FilePath path = m_binaryWriter.path();

		open(path, OpenMode::Trunc, m_encoding);
	}

	void TextWriter::TextWriterDetail::write(const StringView s)
	{
		switch (m_encoding)
		{
		case TextEncoding::UTF16LE:
		case TextEncoding::UTF16BE:
			{
				Array<char16> buffer(Arg::reserve = s.size());

				UTF32toUTF16_Converter translator;

				for (const char32 codePoint : s)
				{
					if (codePoint == U'\r')
					{
						continue;
					}

					if (codePoint == U'\n')
					{
						buffer.push_back(u'\r');
						buffer.push_back(u'\n');
						continue;
					}

					if (const size_t length = translator.put(codePoint);
						length == 1)
					{
						buffer.push_back(translator.get()[0]);
					}
					else
					{
						buffer.push_back(translator.get()[0]);
						buffer.push_back(translator.get()[1]);
					}
				}

				if (m_encoding == TextEncoding::UTF16BE)
				{
					for (char16& ch : buffer)
					{
						ch = SwapEndian(static_cast<uint16>(ch));
					}
				}

				m_binaryWriter.write(buffer.data(), buffer.size_bytes());

				break;
			}
		default:
			{
				Array<char8> buffer(Arg::reserve = s.size());

				UTF32toUTF8_Converter translator;

				for (const char32 codePoint : s)
				{
					if (codePoint == U'\r')
					{
						continue;
					}

					if (codePoint == U'\n')
					{
						buffer.push_back('\r');
						buffer.push_back('\n');
						continue;
					}

					if (const size_t length = translator.put(codePoint);
						length == 1)
					{
						buffer.push_back(translator.get()[0]);
					}
					else
					{
						buffer.append(translator.get().begin(), (translator.get().begin() + length));
					}
				}

				m_binaryWriter.write(buffer.data(), buffer.size_bytes());

				break;
			}
		}
	}

	void TextWriter::TextWriterDetail::writeNewLine()
	{
		switch (m_encoding)
		{
		case TextEncoding::UTF16LE:
			{
				m_binaryWriter.write(u"\r\n", 4);
				break;
			}
		case TextEncoding::UTF16BE:
			{
				const uint8 newLine[] = { 0x00, 0x0D, 0x00, 0x0A };
				m_binaryWriter.write(newLine);
				break;
			}
		default:
			{
				m_binaryWriter.write("\r\n", 2);
				break;
			}
		}
	}

	void TextWriter::TextWriterDetail::writeUTF8(const std::string_view s)
	{
		switch (m_encoding)
		{
		case TextEncoding::UTF16LE:
		case TextEncoding::UTF16BE:
			{
				const std::u16string utf16 = Unicode::UTF8ToUTF16(s);

				Array<char16> buffer(Arg::reserve = utf16.size());

				for (const char16 ch : utf16)
				{
					if (ch == u'\r')
					{
						continue;
					}

					if (ch == u'\n')
					{
						buffer.push_back(u'\r');
						buffer.push_back(u'\n');
						continue;
					}

					buffer.push_back(ch);
				}

				if (m_encoding == TextEncoding::UTF16BE)
				{
					for (char16& ch : buffer)
					{
						ch = SwapEndian(static_cast<uint16>(ch));
					}
				}

				m_binaryWriter.write(buffer.data(), buffer.size_bytes());

				break;
			}
		default:
			{
				Array<char8> buffer(Arg::reserve = s.size());

				for (const char8 ch : s)
				{
					if (ch == '\r')
					{
						continue;
					}

					if (ch == '\n')
					{
						buffer.push_back('\r');
						buffer.push_back('\n');
						continue;
					}

					buffer.push_back(ch);
				}

				m_binaryWriter.write(buffer.data(), buffer.size_bytes());

				break;
			}
		}
	}

	TextEncoding TextWriter::TextWriterDetail::encoding() const noexcept
	{
		return m_encoding;
	}

	const FilePath& TextWriter::TextWriterDetail::path() const noexcept
	{
		return m_binaryWriter.path();
	}
}

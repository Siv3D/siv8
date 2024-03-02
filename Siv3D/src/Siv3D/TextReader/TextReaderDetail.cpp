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

# include "TextReaderDetail.hpp"
# include <Siv3D/BinaryReader.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Endian.hpp>
# include <Siv3D/UnicodeConverter.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Array<std::string> SplitLines(const std::string& s)
		{
			if (s.empty())
			{
				return{};
			}

			Array<std::string> result;

			size_t start = 0;

			for (size_t i = 0; i < s.length(); ++i)
			{
				if (s[i] == '\n')
				{
					result.push_back(s.substr(start, (i - start)));
					start = (i + 1);
				}
			}

			result.push_back(s.substr(start));

			return result;
		}
	}

	TextReader::TextReaderDetail::~TextReaderDetail()
	{
		close();
	}

	bool TextReader::TextReaderDetail::open(const FilePathView path, const Optional<TextEncoding>& encoding)
	{
		if (m_info.isOpen)
		{
			close();
		}

		std::unique_ptr<IReader> tmpReader = std::make_unique<BinaryReader>(path);

		if (not tmpReader->isOpen())
		{
			return false;
		}

		m_reader = std::move(tmpReader);

		m_info =
		{
			.fullPath = FileSystem::FullPath(path),
			.encoding = (encoding ? *encoding : Unicode::GetTextEncoding(*m_reader)),
			.isOpen = true
		};

		if (const int32 bomSize = Unicode::GetBOMSize(m_info.encoding))
		{
			m_reader->skip(bomSize);
		}

		return true;
	}

	bool TextReader::TextReaderDetail::open(std::unique_ptr<IReader>&& reader, const Optional<TextEncoding>& encoding)
	{
		if (m_info.isOpen)
		{
			close();
		}

		if (not reader)
		{
			return false;
		}

		if (not reader->isOpen())
		{
			return false;
		}

		m_reader = std::move(reader);

		m_info =
		{
			.fullPath = {},
			.encoding = (encoding ? *encoding : Unicode::GetTextEncoding(*m_reader)),
			.isOpen = true
		};

		if (const int32 bomSize = Unicode::GetBOMSize(m_info.encoding))
		{
			m_reader->skip(bomSize);
		}

		return true;
	}

	void TextReader::TextReaderDetail::close()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		m_reader.reset();

		m_info = {};
	}

	bool TextReader::TextReaderDetail::isOpen() const noexcept
	{
		return m_info.isOpen;
	}

	bool TextReader::TextReaderDetail::readChar(char32& ch)
	{
		ch = U'\0';

		if (not m_info.isOpen)
		{
			return false;
		}

		for (;;)
		{
			char32 codePoint;

			if (not readCodePoint(codePoint))
			{
				return false;
			}

			if (codePoint == U'\0')
			{
				return false;
			}
			else if (codePoint != U'\r')
			{
				ch = codePoint;
				return true;
			}
		}
	}

	bool TextReader::TextReaderDetail::readLine(std::string& line)
	{
		line.clear();

		if (not m_info.isOpen)
		{
			return false;
		}

		switch (m_info.encoding)
		{
		case TextEncoding::UTF16LE:
			return readLineUTF16LE(line);
		case TextEncoding::UTF16BE:
			return readLineUTF16BE(line);
		default:
			return readLineUTF8(line);
		}
	}

	bool TextReader::TextReaderDetail::readLine(String& line)
	{
		line.clear();

		if (not m_info.isOpen)
		{
			return false;
		}

		bool eof = true;

		for (;;)
		{
			char32 codePoint;

			if (not readCodePoint(codePoint))
			{
				break;
			}

			eof = false;

			if ((codePoint == U'\n') || (codePoint == U'\0'))
			{
				break;
			}
			else if (codePoint != U'\r')
			{
				line.push_back(codePoint);
			}
		}

		return (not eof);
	}

	bool TextReader::TextReaderDetail::readLines(Array<std::string>& lines)
	{
		lines.clear();

		if (m_info.encoding == TextEncoding::UTF16LE)
		{
			return readLinesUTF16LE(lines);
		}
		else if (m_info.encoding == TextEncoding::UTF16BE)
		{
			return readLinesUTF16BE(lines);
		}
		else
		{
			std::string s8;

			if (not readAll(s8))
			{
				return false;
			}

			lines = SplitLines(s8);

			return true;
		}
	}

	bool TextReader::TextReaderDetail::readLines(Array<String>& lines)
	{
		lines.clear();

		if (not m_info.isOpen)
		{
			return false;
		}

		String line;

		bool eof = true;

		for (;;)
		{
			char32 codePoint;

			if (not readCodePoint(codePoint))
			{
				if (not eof)
				{
					lines.push_back(std::move(line));
				}

				break;
			}

			eof = false;

			if ((codePoint == U'\n') || (codePoint == U'\0'))
			{
				lines.push_back(line);
				line.clear();
			}
			else if (codePoint != U'\r')
			{
				line.push_back(codePoint);
			}
		}

		return (not eof);
	}

	bool TextReader::TextReaderDetail::readAll(std::string& s)
	{
		s.clear();

		if (not m_info.isOpen)
		{
			return false;
		}

		switch (m_info.encoding)
		{
		case TextEncoding::UTF16LE:
			return readAllUTF16LE(s);
		case TextEncoding::UTF16BE:
			return readAllUTF16BE(s);
		default:
			return readAllUTF8(s);
		}
	}

	bool TextReader::TextReaderDetail::readAll(String& s)
	{
		s.clear();

		if ((m_info.encoding == TextEncoding::UTF16LE)
			|| (m_info.encoding == TextEncoding::UTF16BE))
		{
			if (not m_info.isOpen)
			{
				return false;
			}

			bool eof = true;

			for (;;)
			{
				char32 codePoint;

				if (not readCodePoint(codePoint))
				{
					break;
				}

				eof = false;

				if (codePoint == U'\0')
				{
					break;
				}
				else if (codePoint != U'\r')
				{
					s.push_back(codePoint);
				}
			}

			return (not eof);
		}
		else
		{
			if (std::string s8; readAll(s8))
			{
				s = Unicode::FromUTF8(s8);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	TextEncoding TextReader::TextReaderDetail::encoding() const noexcept
	{
		return m_info.encoding;
	}

	const FilePath& TextReader::TextReaderDetail::path() const noexcept
	{
		return m_info.fullPath;
	}

	bool TextReader::TextReaderDetail::readCodePoint(char32& codePoint)
	{
		switch (m_info.encoding)
		{
		case TextEncoding::UTF16LE:
			return readCodePointUTF16LE(codePoint);
		case TextEncoding::UTF16BE:
			return readCodePointUTF16BE(codePoint);
		default:
			return readCodePointUTF8(codePoint);
		}
	}

	bool TextReader::TextReaderDetail::readCodePointUTF8(char32& codePoint)
	{
		uint8 cx;

		if (not readByte(cx))
		{
			return false;
		}

		UTF8toUTF32_Converter converter;

		if (converter.put(cx)) // 1
		{
			codePoint = converter.get();
			return true;
		}

		if (not readByte(cx))
		{
			return false;
		}

		if (converter.put(cx)) // 2
		{
			codePoint = converter.get();
			return true;
		}

		if (not readByte(cx))
		{
			return false;
		}

		if (converter.put(cx)) // 3
		{
			codePoint = converter.get();
			return true;
		}

		if (not readByte(cx))
		{
			return false;
		}

		if (converter.put(cx)) // 4
		{
			codePoint = converter.get();
			return true;
		}

		return false;
	}

	bool TextReader::TextReaderDetail::readCodePointUTF16LE(char32& codePoint)
	{
		uint16 c0 = 0, c1 = 0;

		if (not readTwoBytes(c0))
		{
			return false;
		}

		if (Unicode::IsHighSurrogate(c0))
		{
			if (not readTwoBytes(c1))
			{
				return false;
			}

			codePoint = ((((c0 - 0xD800) << 10) | (c1 - 0xDC00)) + 0x10000);
		}
		else
		{
			codePoint = c0;
		}

		return true;
	}

	bool TextReader::TextReaderDetail::readCodePointUTF16BE(char32& codePoint)
	{
		uint16 c0 = 0, c1 = 0;

		if (not readTwoBytes(c0))
		{
			return false;
		}

		c0 = SwapEndian(c0);

		if (Unicode::IsHighSurrogate(c0))
		{
			if (not readTwoBytes(c1))
			{
				return false;
			}

			c1 = SwapEndian(c1);

			codePoint = ((((c0 - 0xD800) << 10) | (c1 - 0xDC00)) + 0x10000);
		}
		else
		{
			codePoint = c0;
		}

		return true;
	}

	bool TextReader::TextReaderDetail::readByte(uint8& c)
	{
		return m_reader->read(c);
	}

	bool TextReader::TextReaderDetail::readTwoBytes(uint16& c)
	{
		return m_reader->read(c);
	}

	bool TextReader::TextReaderDetail::readLineUTF8(std::string& line)
	{
		bool eof = true;

		for (;;)
		{
			uint8 ch;

			if (not readByte(ch))
			{
				break;
			}

			eof = false;

			if ((ch == '\n') || (ch == '\0'))
			{
				break;
			}
			else if (ch != '\r')
			{
				line.push_back(ch);
			}
		}

		return (not eof);
	}

	bool TextReader::TextReaderDetail::readLineUTF16LE(std::string& line)
	{
		// [Siv3D ToDo] バッファを使わないようにする
		std::u16string line16;

		bool eof = true;

		for (;;)
		{
			uint16 c0;

			if (not readTwoBytes(c0))
			{
				break;
			}

			eof = false;

			if (Unicode::IsHighSurrogate(c0))
			{
				uint16 c1;

				if (not readTwoBytes(c1))
				{
					break;
				}

				line16.push_back(c0);
				line16.push_back(c1);
			}
			else
			{
				if (c0 == u'\n' || c0 == u'\0')
				{
					break;
				}
				else if (c0 != u'\r')
				{
					line16.push_back(c0);
				}
			}
		}

		line = Unicode::UTF16ToUTF8(line16);

		return (not eof);
	}

	bool TextReader::TextReaderDetail::readLineUTF16BE(std::string& line)
	{
		// [Siv3D ToDo] バッファを使わないようにする
		std::u16string line16;

		bool eof = true;

		for (;;)
		{
			uint16 c0;

			if (not readTwoBytes(c0))
			{
				break;
			}

			eof = false;

			c0 = SwapEndian(c0);

			if (Unicode::IsHighSurrogate(c0))
			{
				uint16 c1;

				if (not readTwoBytes(c1))
				{
					break;
				}

				c1 = SwapEndian(c1);

				line16.push_back(c0);
				line16.push_back(c1);
			}
			else
			{
				if (c0 == u'\n' || c0 == u'\0')
				{
					break;
				}
				else if (c0 != u'\r')
				{
					line16.push_back(c0);
				}
			}
		}

		line = Unicode::UTF16ToUTF8(line16);

		return (not eof);
	}

	bool TextReader::TextReaderDetail::readLinesUTF16LE(Array<std::string>& lines)
	{
		std::u16string line16;

		bool eof = true;

		for (;;)
		{
			uint16 c0;

			if (not readTwoBytes(c0))
			{
				if (not eof)
				{
					lines.push_back(Unicode::UTF16ToUTF8(line16));
				}

				break;
			}

			eof = false;

			if (Unicode::IsHighSurrogate(c0))
			{
				uint16 c1;

				if (not readTwoBytes(c1))
				{
					break;
				}

				line16.push_back(c0);
				line16.push_back(c1);
			}
			else
			{
				if ((c0 == u'\n') || (c0 == u'\0'))
				{
					lines.push_back(Unicode::UTF16ToUTF8(line16));
					line16.clear();
				}
				else if (c0 != u'\r')
				{
					line16.push_back(c0);
				}
			}
		}

		return (not eof);
	}

	bool TextReader::TextReaderDetail::readLinesUTF16BE(Array<std::string>& lines)
	{
		std::u16string line16;

		bool eof = true;

		for (;;)
		{
			uint16 c0;

			if (not readTwoBytes(c0))
			{
				if (not eof)
				{
					lines.push_back(Unicode::UTF16ToUTF8(line16));
				}

				break;
			}

			eof = false;

			c0 = SwapEndian(c0);

			if (Unicode::IsHighSurrogate(c0))
			{
				uint16 c1;

				if (not readTwoBytes(c1))
				{
					break;
				}

				c1 = SwapEndian(c1);

				line16.push_back(c0);
				line16.push_back(c1);
			}
			else
			{
				if ((c0 == u'\n') || (c0 == u'\0'))
				{
					lines.push_back(Unicode::UTF16ToUTF8(line16));
					line16.clear();
				}
				else if (c0 != u'\r')
				{
					line16.push_back(c0);
				}
			}
		}

		return (not eof);
	}

	bool TextReader::TextReaderDetail::readAllUTF8(std::string& s)
	{
		const int64 readSize = (m_reader->size() - m_reader->getPos());

		s.resize(readSize);

		const int64 readBytes = m_reader->read(s.data(), readSize);

		if (readBytes < readSize)
		{
			s.resize(readBytes);
		}

		s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());

		return (0 < readBytes);
	}

	bool TextReader::TextReaderDetail::readAllUTF16LE(std::string& s)
	{
		std::u16string line;

		bool eof = true;

		for (;;)
		{
			uint16 c0;

			if (not readTwoBytes(c0))
			{
				if (not eof)
				{
					s.append(Unicode::UTF16ToUTF8(line));
				}

				break;
			}

			eof = false;

			if (Unicode::IsHighSurrogate(c0))
			{
				uint16 c1;

				if (not readTwoBytes(c1))
				{
					break;
				}

				line.push_back(c0);
				line.push_back(c1);
			}
			else
			{
				if (c0 == u'\n')
				{
					line.push_back(c0);
					s.append(Unicode::UTF16ToUTF8(line));
					line.clear();
				}
				else if (c0 != u'\r')
				{
					line.push_back(c0);
				}
			}
		}

		return (not eof);
	}

	bool TextReader::TextReaderDetail::readAllUTF16BE(std::string& s)
	{
		std::u16string line;

		bool eof = true;

		for (;;)
		{
			uint16 c0;

			if (not readTwoBytes(c0))
			{
				if (not eof)
				{
					s.append(Unicode::UTF16ToUTF8(line));
				}

				break;
			}

			eof = false;

			c0 = SwapEndian(c0);

			if (Unicode::IsHighSurrogate(c0))
			{
				uint16 c1;

				if (not readTwoBytes(c1))
				{
					break;
				}

				c1 = SwapEndian(c1);

				line.push_back(c0);
				line.push_back(c1);
			}
			else
			{
				if (c0 == u'\n')
				{
					line.push_back(c0);
					s.append(Unicode::UTF16ToUTF8(line));
					line.clear();
				}
				else if (c0 != u'\r')
				{
					line.push_back(c0);
				}
			}
		}

		return (not eof);
	}
}

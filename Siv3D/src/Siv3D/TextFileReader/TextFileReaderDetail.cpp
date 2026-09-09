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

# include "TextFileReaderDetail.hpp"
# include <Siv3D/BinaryFileReader.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Endian.hpp>
# include <Siv3D/UnicodeConverter.hpp>
# include <cstring>
# include <exception>

namespace s3d
{
	namespace
	{
		static void SplitLines(const std::string& s, Array<std::string>& lines)
		{
			lines.clear();

			if (s.empty())
			{
				return;
			}

			size_t start = 0;

			for (size_t end; (end = s.find('\n', start)) != std::string::npos; start = (end + 1))
			{
				lines.push_back(s.substr(start, (end - start)));
			}

			lines.push_back(s.substr(start));
		}

		static void SplitLines(const std::string_view s, Array<String>& lines)
		{
			lines.clear();

			if (s.empty())
			{
				return;
			}

			size_t start = 0;

			for (size_t end; (end = s.find('\n', start)) != std::string_view::npos; start = (end + 1))
			{
				lines.push_back(Unicode::FromUTF8(s.substr(start, (end - start))));
			}

			lines.push_back(Unicode::FromUTF8(s.substr(start)));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	TextFileReader::TextFileReaderDetail::~TextFileReaderDetail()
	{
		close();
	}

	////////////////////////////////////////////////////////////////
	//
	//	open
	//
	////////////////////////////////////////////////////////////////

	bool TextFileReader::TextFileReaderDetail::open(const FilePathView path, const Optional<TextEncoding>& encoding)
	{
		if (m_info.isOpen)
		{
			close();
		}

		std::unique_ptr<IReader> tmpReader = std::make_unique<BinaryFileReader>(path);

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

	bool TextFileReader::TextFileReaderDetail::open(std::unique_ptr<IReader> reader, const Optional<TextEncoding>& encoding)
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

	////////////////////////////////////////////////////////////////
	//
	//	close
	//
	////////////////////////////////////////////////////////////////

	void TextFileReader::TextFileReaderDetail::close()
	{
		if (not m_info.isOpen)
		{
			return;
		}

		m_reader.reset();
		m_utf8BufferPos = 0;
		m_utf8BufferLength = 0;

		m_info = {};
	}

	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	bool TextFileReader::TextFileReaderDetail::isOpen() const noexcept
	{
		return m_info.isOpen;
	}

	////////////////////////////////////////////////////////////////
	//
	//	readChar
	//
	////////////////////////////////////////////////////////////////

	bool TextFileReader::TextFileReaderDetail::readChar(char32& ch)
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

	////////////////////////////////////////////////////////////////
	//
	//	readLine
	//
	////////////////////////////////////////////////////////////////

	bool TextFileReader::TextFileReaderDetail::readLine(std::string& line)
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

	bool TextFileReader::TextFileReaderDetail::readLine(String& line)
	{
		line.clear();

		if (not m_info.isOpen)
		{
			return false;
		}

		const auto read = [&line](auto readCodePoint)
		{
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
		};

		switch (m_info.encoding)
		{
		case TextEncoding::UTF16LE:
			return read([this](char32& ch) { return readCodePointUTF16LE(ch); });
		case TextEncoding::UTF16BE:
			return read([this](char32& ch) { return readCodePointUTF16BE(ch); });
		default:
			return read([this](char32& ch) { return readCodePointUTF8(ch); });
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	readLines
	//
	////////////////////////////////////////////////////////////////

	bool TextFileReader::TextFileReaderDetail::readLines(Array<std::string>& lines)
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

			SplitLines(s8, lines);

			return true;
		}
	}

	bool TextFileReader::TextFileReaderDetail::readLines(Array<String>& lines)
	{
		lines.clear();

		if ((m_info.encoding == TextEncoding::UTF16LE)
			|| (m_info.encoding == TextEncoding::UTF16BE))
		{
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
		else
		{
			std::string s8;

			if (not readAll(s8))
			{
				return false;
			}

			SplitLines(s8, lines);

			return true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	readAll
	//
	////////////////////////////////////////////////////////////////

	bool TextFileReader::TextFileReaderDetail::readAll(std::string& s)
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

	bool TextFileReader::TextFileReaderDetail::readAll(String& s)
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
				Unicode::FromUTF8(s8, s);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	encoding
	//
	////////////////////////////////////////////////////////////////

	TextEncoding TextFileReader::TextFileReaderDetail::encoding() const noexcept
	{
		return m_info.encoding;
	}

	////////////////////////////////////////////////////////////////
	//
	//	path
	//
	////////////////////////////////////////////////////////////////

	const FilePath& TextFileReader::TextFileReaderDetail::path() const noexcept
	{
		return m_info.fullPath;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	bool TextFileReader::TextFileReaderDetail::readCodePoint(char32& codePoint)
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

	bool TextFileReader::TextFileReaderDetail::readCodePointUTF8(char32& codePoint)
	{
		uint8 byte;

		if (not readByte(byte))
		{
			return false;
		}

		if (byte < 0x80)
		{
			codePoint = byte;
			return true;
		}

		UTF8toUTF32_Converter converter;

		do
		{
			const auto result = converter.put(static_cast<char8>(byte));

			if (result.status == UnicodeDecodeStatus::NeedMore)
			{
				continue;
			}

			if (not result.consumed)
			{
				// Leave the next code unit available to all reader methods.
				--m_utf8BufferPos;
			}

			codePoint = converter.get();
			return true;
		} while (readByte(byte));

		if (converter.finish())
		{
			codePoint = converter.get();
			return true;
		}

		return false;
	}

	bool TextFileReader::TextFileReaderDetail::readCodePointUTF16LE(char32& codePoint)
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

	bool TextFileReader::TextFileReaderDetail::readCodePointUTF16BE(char32& codePoint)
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

	bool TextFileReader::TextFileReaderDetail::readByte(uint8& c)
	{
		if ((m_utf8BufferPos == m_utf8BufferLength) && (not refillUTF8Buffer()))
		{
			return false;
		}

		c = m_utf8Buffer[m_utf8BufferPos++];
		return true;
	}

	bool TextFileReader::TextFileReaderDetail::refillUTF8Buffer()
	{
		m_utf8BufferPos = 0;
		m_utf8BufferLength = 0;

		if (not m_utf8Buffer)
		{
			m_utf8Buffer = std::make_unique_for_overwrite<uint8[]>(UTF8BufferSize);
		}

		m_utf8BufferLength = static_cast<size_t>(m_reader->read(m_utf8Buffer.get(), UTF8BufferSize));
		return (0 < m_utf8BufferLength);
	}

	bool TextFileReader::TextFileReaderDetail::readTwoBytes(uint16& c)
	{
		return m_reader->read(c);
	}

	bool TextFileReader::TextFileReaderDetail::readLineUTF8(std::string& line)
	{
		bool eof = true;

		for (;;)
		{
			if ((m_utf8BufferPos == m_utf8BufferLength) && (not refillUTF8Buffer()))
			{
				return (not eof);
			}

			eof = false;
			const size_t start = m_utf8BufferPos;
			size_t end = start;
			while (end < m_utf8BufferLength)
			{
				const uint8 ch = m_utf8Buffer[end];
				if ((ch == '\r') || (ch == '\n') || (ch == '\0'))
				{
					break;
				}
				++end;
			}

			line.append(reinterpret_cast<const char*>(m_utf8Buffer.get() + start), (end - start));
			m_utf8BufferPos = end;

			if (end < m_utf8BufferLength)
			{
				++m_utf8BufferPos;
				if (m_utf8Buffer[end] != '\r')
				{
					return true;
				}
			}
		}
	}

	bool TextFileReader::TextFileReaderDetail::readLineUTF16LE(std::string& line)
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

	bool TextFileReader::TextFileReaderDetail::readLineUTF16BE(std::string& line)
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

	bool TextFileReader::TextFileReaderDetail::readLinesUTF16LE(Array<std::string>& lines)
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

	bool TextFileReader::TextFileReaderDetail::readLinesUTF16BE(Array<std::string>& lines)
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

	bool TextFileReader::TextFileReaderDetail::readAllUTF8(std::string& s)
	{
		const size_t buffered = (m_utf8BufferLength - m_utf8BufferPos);
		const int64 readSize = (m_reader->size() - m_reader->getPos());

		size_t readBytes = 0;
		std::exception_ptr readException;
		s.resize_and_overwrite((buffered + static_cast<size_t>(readSize)), [&](char* dst, size_t) noexcept -> size_t
			{
				try
				{
					if (buffered)
					{
						std::memcpy(dst, (m_utf8Buffer.get() + m_utf8BufferPos), buffered);
						m_utf8BufferPos = m_utf8BufferLength;
					}

					readBytes = buffered;
					if (readSize)
					{
						// Whole-file reads go directly to the destination; only an unread
						// prefix from an earlier readChar/readLine needs to be copied.
						readBytes += static_cast<size_t>(m_reader->read((dst + buffered), readSize));
					}
					return readBytes;
				}
				catch (...)
				{
					// resize_and_overwrite requires a non-throwing operation, but
					// an IReader can throw after writing part of the destination.
					readException = std::current_exception();
					return 0;
				}
			});

		if (readException)
		{
			std::rethrow_exception(readException);
		}

		s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());

		return (0 < readBytes);
	}

	bool TextFileReader::TextFileReaderDetail::readAllUTF16LE(std::string& s)
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

	bool TextFileReader::TextFileReaderDetail::readAllUTF16BE(std::string& s)
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

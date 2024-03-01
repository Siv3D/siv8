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


	Optional<char32> TextReader::TextReaderDetail::readChar()
	{
		if (not m_info.isOpen)
		{
			return none;
		}

		for (;;)
		{
			char32 codePoint;

			if (not readCodePoint(codePoint))
			{
				return none;
			}

			if (codePoint == U'\0')
			{
				return none;
			}
			else if (codePoint != U'\r')
			{
				return codePoint;
			}
		}
	}




















	bool TextReader::TextReaderDetail::readUTF8(char32& c)
	{
		uint8 cx;

		if (not readByte(cx))
		{
			return false;
		}

		UTF8toUTF32_Converter converter;

		if (converter.put(cx)) // 1
		{
			c = converter.get();
			return true;
		}

		if (not readByte(cx))
		{
			return false;
		}

		if (converter.put(cx)) // 2
		{
			c = converter.get();
			return true;
		}

		if (not readByte(cx))
		{
			return false;
		}

		if (converter.put(cx)) // 3
		{
			c = converter.get();
			return true;
		}

		if (not readByte(cx))
		{
			return false;
		}

		if (converter.put(cx)) // 4
		{
			c = converter.get();
			return true;
		}

		return false;
	}

	bool TextReader::TextReaderDetail::readUTF16LE(char32& c)
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

			c = ((((c0 - 0xD800) << 10) | (c1 - 0xDC00)) + 0x10000);
		}
		else
		{
			c = c0;
		}

		return true;
	}

	bool TextReader::TextReaderDetail::readUTF16BE(char32& c)
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

			c = ((((c0 - 0xD800) << 10) | (c1 - 0xDC00)) + 0x10000);
		}
		else
		{
			c = c0;
		}

		return true;
	}

	bool TextReader::TextReaderDetail::readCodePoint(char32& codePoint)
	{
		switch (m_info.encoding)
		{
		case TextEncoding::UTF16LE:
			return readUTF16LE(codePoint);
		case TextEncoding::UTF16BE:
			return readUTF16BE(codePoint);
		default:
			return readUTF8(codePoint);
		}
	}
}

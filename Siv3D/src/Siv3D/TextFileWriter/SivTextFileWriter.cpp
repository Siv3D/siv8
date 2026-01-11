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

# include <Siv3D/TextFileWriter.hpp>
# include "TextFileWriterDetail.hpp"

namespace s3d
{
	namespace detail
	{
		TextFileWriterBuffer::TextFileWriterBuffer(TextFileWriter& writer)
			: formatData{ std::make_unique<FormatData>() }
			, m_writer{ writer }
			, m_isLast{ true } {}

		TextFileWriterBuffer::TextFileWriterBuffer(TextFileWriterBuffer&& other) noexcept
			: formatData{ std::move(other.formatData) }
			, m_writer{ other.m_writer }
			, m_isLast{ true }
		{
			other.m_isLast = false;
		}

		TextFileWriterBuffer::~TextFileWriterBuffer()
		{
			if (m_isLast)
			{
				m_writer.writeln(formatData->string);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	TextFileWriter::TextFileWriter()
		: pImpl{ std::make_shared<TextFileWriterDetail>() } {}

	TextFileWriter::TextFileWriter(const FilePathView path, const TextEncoding encoding)
		: TextFileWriter{}
	{
		open(path, OpenMode::Trunc, encoding);
	}

	TextFileWriter::TextFileWriter(const FilePathView path, const OpenMode openMode, const TextEncoding encoding)
		: TextFileWriter{}
	{
		open(path, openMode, encoding);
	}

	////////////////////////////////////////////////////////////////
	//
	//	open
	//
	////////////////////////////////////////////////////////////////

	bool TextFileWriter::open(const FilePathView path, const TextEncoding encoding)
	{
		return open(path, OpenMode::Trunc, encoding);
	}

	bool TextFileWriter::open(const FilePathView path, const OpenMode openMode, const TextEncoding encoding)
	{
		return pImpl->open(path, openMode, encoding);
	}

	////////////////////////////////////////////////////////////////
	//
	//	close
	//
	////////////////////////////////////////////////////////////////

	void TextFileWriter::close()
	{
		pImpl->close();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	bool TextFileWriter::isOpen() const noexcept
	{
		return pImpl->isOpen();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	TextFileWriter::operator bool() const noexcept
	{
		return pImpl->isOpen();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void TextFileWriter::clear()
	{
		pImpl->clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	write
	//
	////////////////////////////////////////////////////////////////

	void TextFileWriter::write(const char32 ch)
	{
		pImpl->write(StringView(&ch, 1));
	}

	void TextFileWriter::write(const char32* s)
	{
		pImpl->write(StringView(s));
	}

	void TextFileWriter::write(const String& s)
	{
		pImpl->write(StringView(s));
	}

	void TextFileWriter::write(const StringView s)
	{
		pImpl->write(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	writeln
	//
	////////////////////////////////////////////////////////////////

	void TextFileWriter::writeln(const char32 ch)
	{
		writeln(StringView(&ch, 1));
	}

	void TextFileWriter::writeln(const char32* s)
	{
		writeln(StringView(s));
	}

	void TextFileWriter::writeln(const String& s)
	{
		writeln(StringView(s));
	}

	void TextFileWriter::writeln(const StringView s)
	{
		pImpl->write(s);

		pImpl->writeNewLine();
	}

	////////////////////////////////////////////////////////////////
	//
	//	writeUTF8
	//
	////////////////////////////////////////////////////////////////

	void TextFileWriter::writeUTF8(const std::string_view s)
	{
		pImpl->writeUTF8(s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	writelnUTF8
	//
	////////////////////////////////////////////////////////////////

	void TextFileWriter::writelnUTF8(const std::string_view s)
	{
		pImpl->writeUTF8(s);

		pImpl->writeNewLine();
	}

	////////////////////////////////////////////////////////////////
	//
	//	encoding
	//
	////////////////////////////////////////////////////////////////

	TextEncoding TextFileWriter::encoding() const noexcept
	{
		return pImpl->encoding();
	}

	////////////////////////////////////////////////////////////////
	//
	//	path
	//
	////////////////////////////////////////////////////////////////

	const FilePath& TextFileWriter::path() const noexcept
	{
		return pImpl->path();
	}
}

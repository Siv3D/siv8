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

# pragma once

namespace s3d
{
	namespace detail
	{
		TextFileWriterBuffer& TextFileWriterBuffer::operator <<(const Concept::Formattable auto& value)
		{
			Formatter(*formatData, value);

			return *this;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	write
	//
	////////////////////////////////////////////////////////////////

	void TextFileWriter::write(const Concept::Formattable auto& ... args)
	{
		write(Format(args...));
	}

	////////////////////////////////////////////////////////////////
	//
	//	writeln
	//
	////////////////////////////////////////////////////////////////

	void TextFileWriter::writeln(const Concept::Formattable auto& ... args)
	{
		writeln(Format(args...));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	detail::TextFileWriterBuffer TextFileWriter::operator <<(const Concept::Formattable auto& value)
	{
		detail::TextFileWriterBuffer buffer{ *this };

		Formatter(*(buffer.formatData), value);

		return buffer;
	}
}

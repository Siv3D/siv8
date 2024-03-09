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

# pragma once

namespace s3d
{
	namespace detail
	{
		TextWriterBuffer& TextWriterBuffer::operator <<(const Concept::Formattable auto& value)
		{
			Formatter(*formatData, value);

			return *this;
		}
	}

	void TextWriter::write(const Concept::Formattable auto& ... args)
	{
		write(Format(args...));
	}

	void TextWriter::writeln(const Concept::Formattable auto& ... args)
	{
		writeln(Format(args...));
	}

	detail::TextWriterBuffer TextWriter::operator <<(const Concept::Formattable auto& value)
	{
		detail::TextWriterBuffer buffer{ *this };

		Formatter(*(buffer.formatData), value);

		return buffer;
	}
}

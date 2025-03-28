﻿//-----------------------------------------------
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
		TextWriterBuffer& TextWriterBuffer::operator <<(const Concept::Formattable auto& value)
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

	void TextWriter::write(const Concept::Formattable auto& ... args)
	{
		write(Format(args...));
	}

	////////////////////////////////////////////////////////////////
	//
	//	writeln
	//
	////////////////////////////////////////////////////////////////

	void TextWriter::writeln(const Concept::Formattable auto& ... args)
	{
		writeln(Format(args...));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	detail::TextWriterBuffer TextWriter::operator <<(const Concept::Formattable auto& value)
	{
		detail::TextWriterBuffer buffer{ *this };

		Formatter(*(buffer.formatData), value);

		return buffer;
	}
}

﻿//-----------------------------------------------
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
# include <Siv3D/TextWriter.hpp>
# include <Siv3D/BinaryWriter.hpp>

namespace s3d
{
	class TextWriter::TextWriterDetail
	{
	public:

		TextWriterDetail() = default;

		~TextWriterDetail();

		bool open(FilePathView path, OpenMode openMode, TextEncoding encoding);

		void close();

		[[nodiscard]]
		bool isOpen() const noexcept;

		void clear();

		void write(StringView s);

		void writeNewLine();

		void writeUTF8(std::string_view s);

		[[nodiscard]]
		TextEncoding encoding() const noexcept;

		[[nodiscard]]
		const FilePath& path() const noexcept;

	private:

		BinaryWriter m_binaryWriter;

		TextEncoding m_encoding = TextEncoding::Default;
	};
}

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
# include <Siv3D/TextFileWriter.hpp>
# include <Siv3D/BinaryFileWriter.hpp>

namespace s3d
{
	class TextFileWriter::TextFileWriterDetail
	{
	public:

		TextFileWriterDetail() = default;

		~TextFileWriterDetail();

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

		BinaryFileWriter m_binaryWriter;

		TextEncoding m_encoding = TextEncoding::Default;
	};
}

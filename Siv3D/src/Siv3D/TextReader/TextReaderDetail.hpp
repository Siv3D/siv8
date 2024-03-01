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
# include <Siv3D/TextReader.hpp>

namespace s3d
{
	class TextReader::TextReaderDetail
	{
	public:

		TextReaderDetail() = default;

		~TextReaderDetail();

		[[nodiscard]]
		bool open(FilePathView path, const Optional<TextEncoding>& encoding);

		[[nodiscard]]
		bool open(std::unique_ptr<IReader>&& reader, const Optional<TextEncoding>& encoding);

		void close();

		[[nodiscard]]
		bool isOpen() const noexcept;

		[[nodiscard]]
		bool readChar(char32& ch);

		[[nodiscard]]
		bool readLine(std::string& line);

		[[nodiscard]]
		bool readLine(String& line);

		bool readLines(Array<std::string>& lines);

		bool readLines(Array<String>& lines);

		bool readAll(std::string& s);

		bool readAll(String& s);

		[[nodiscard]]
		TextEncoding encoding() const noexcept;

		[[nodiscard]]
		const FilePath& path() const noexcept;

	private:

		std::unique_ptr<IReader> m_reader;

		struct Info
		{
			FilePath fullPath;
			TextEncoding encoding = TextEncoding::Default;
			bool isOpen = false;
		} m_info;

		[[nodiscard]]
		bool readCodePoint(char32& codePoint);

		[[nodiscard]]
		bool readCodePointUTF8(char32& codePoint);

		[[nodiscard]]
		bool readCodePointUTF16LE(char32& codePoint);

		[[nodiscard]]
		bool readCodePointUTF16BE(char32& codePoint);

		[[nodiscard]]
		bool readByte(uint8& c);

		[[nodiscard]]
		bool readTwoBytes(uint16& c);

		[[nodiscard]]
		bool readLineUTF8(std::string& line);

		[[nodiscard]]
		bool readLineUTF16LE(std::string& line);

		[[nodiscard]]
		bool readLineUTF16BE(std::string& line);

		[[nodiscard]]
		bool readLinesUTF8(Array<std::string>& lines);

		[[nodiscard]]
		bool readLinesUTF16LE(Array<std::string>& lines);

		[[nodiscard]]
		bool readLinesUTF16BE(Array<std::string>& lines);

		[[nodiscard]]
		bool readAllUTF8(std::string& s);

		[[nodiscard]]
		bool readAllUTF16LE(std::string& s);

		[[nodiscard]]
		bool readAllUTF16BE(std::string& s);
	};
}

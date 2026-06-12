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

# pragma once
# include <fstream>
# include <Siv3D/BinaryFileReader.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Byte.hpp>
# include <Siv3D/NonNull.hpp>

namespace s3d
{
	class BinaryFileReader::BinaryFileReaderDetail
	{
	public:

		[[nodiscard]]
		BinaryFileReaderDetail() = default;

		~BinaryFileReaderDetail();

		[[nodiscard]]
		bool open(FilePathView path);

		void close();

		[[nodiscard]]
		bool isOpen() const noexcept;

		[[nodiscard]]
		int64 size() const noexcept;

		int64 setPos(int64 pos);

		int64 skip(int64 offset);

		[[nodiscard]]
		int64 getPos() const;

		[[nodiscard]]
		int64 read(NonNull<void*> dst, int64 readSize);

		[[nodiscard]]
		int64 read(NonNull<void*> dst, int64 pos, int64 readSize);

		[[nodiscard]]
		int64 lookahead(NonNull<void*> dst, int64 readSize);

		[[nodiscard]]
		int64 lookahead(NonNull<void*> dst, int64 pos, int64 readSize);

		[[nodiscard]]
		const FilePath& path() const noexcept;

	private:

		struct Resource
		{
			const Byte* pointer = nullptr;

			int64 readPos = 0;

			int64 read(NonNull<void*> dst, int64 pos, int64 readSize, int64 fileSize);

			int64 lookahead(NonNull<void*> dst, int64 pos, int64 readSize, int64 fileSize);

		} m_resource;

		struct File
		{
			std::ifstream file;

			int64 readPos = 0;

			int64 read(NonNull<void*> dst, int64 readSize, int64 fileSize, const FilePath& fullPath);

			int64 lookahead(NonNull<void*> dst, int64 readSize, int64 fileSize, const FilePath& fullPath);

			int64 lookaheadAt(NonNull<void*> dst, int64 pos, int64 readSize, int64 fileSize, const FilePath& fullPath);

		} m_file;

		struct Info
		{
			FilePath fullPath;

			int64 fileSize = 0;

			bool isOpen = false;

		} m_info;

		[[nodiscard]]
		bool isResource() const noexcept;
	};
}

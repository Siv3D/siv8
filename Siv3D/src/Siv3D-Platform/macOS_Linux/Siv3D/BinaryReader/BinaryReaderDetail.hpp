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

# include <fstream>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/BinaryReader.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Byte.hpp>
# include <Siv3D/NonNull.hpp>

namespace s3d
{
	class BinaryReader::BinaryReaderDetail
	{
	public:

		[[nodiscard]]
		BinaryReaderDetail() = default;

		~BinaryReaderDetail();

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
		int64 getPos();

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

		struct File
		{
			std::ifstream file;

			int64 readPos = 0;

			int64 read(NonNull<void*> dst, int64 readSize, int64 fileSize, const FilePath& fullPath);

			int64 lookahead(NonNull<void*> dst, int64 readSize, int64 fileSize, const FilePath& fullPath);

		} m_file;

		struct Info
		{
			FilePath fullPath;

			int64 fileSize = 0;

			bool isOpen = false;

		} m_info;
	};
}

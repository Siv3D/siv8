//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <fstream>
# include <Siv3D/BinaryReader.hpp>
# include <Siv3D/NonNull.hpp>
# include <Siv3D/Byte.hpp>
# include <Siv3D/String.hpp>

namespace s3d
{
	class BinaryReader::BinaryReaderDetail
	{
	public:

		[[nodiscard]]
		BinaryReaderDetail();

		~BinaryReaderDetail();

		[[nodiscard]]
		bool open(FilePathView path);

		void close();

		[[nodiscard]]
		bool isOpen() const noexcept;

		[[nodiscard]]
		int64 size() const noexcept;

		[[nodiscard]]
		int64 setPos(int64 clampedPos);

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
		} m_file;

		struct Resource
		{
			const Byte* pointer = nullptr;
			int64 readPos = 0;
		} m_resource;

		struct Info
		{
			FilePath fullPath;
			int64 size = 0;
			bool isOpen = false;
		} m_info;

		[[nodiscard]]
		bool isResource() const noexcept;
	};
}

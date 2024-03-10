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
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/BinaryWriter.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Byte.hpp>
# include <Siv3D/NonNull.hpp>

namespace s3d
{
	class BinaryWriter::BinaryWriterDetail
	{
	public:

		BinaryWriterDetail() = default;

		~BinaryWriterDetail();

		bool open(FilePathView path, OpenMode openMode);

		void close();

		bool isOpen() const noexcept;

		void flush();

		void clear();

		int64 size();

		int64 getPos();

		int64 setPos(int64 clampedPos);

		int64 write(NonNull<const void*> src, size_t writeSize);

		const FilePath& path() const noexcept;

	private:

		constexpr static size_t BufferSize = 8192;

		struct File
		{
			HANDLE handle = INVALID_HANDLE_VALUE;

			void close();

		} m_file;

		struct Buffer
		{
			std::unique_ptr<Byte[]> data;

			size_t writePos = 0;

			[[nodiscard]]
			size_t available() const noexcept;

		} m_buffer;

		struct Info
		{
			FilePath fullPath;

			bool isOpen = false;

		} m_info;

		int64 fillBuffer(NonNull<const void*> src, size_t writeSize);
	};
}

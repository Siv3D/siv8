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

# include <Siv3D/MemoryMappedFileView.hpp>

namespace s3d
{
	class MemoryMappedFileView::MemoryMappedFileViewDetail
	{
	public:

		[[nodiscard]]
		MemoryMappedFileViewDetail() = default;

		~MemoryMappedFileViewDetail();

		bool open(FilePathView path);

		void close();

		bool isOpen() const;

		[[nodiscard]]
		MappedMemoryView map(size_t offset, size_t requestSize);

		void unmap();

		[[nodiscard]]
		int64 size() const;

		[[nodiscard]]
		const FilePath& path() const;

	private:

		struct File
		{
			int fileHandle = -1;

			Byte* baseAddress = nullptr;

			size_t mapLength = 0;

		} m_file;

		struct Info
		{
			FilePath fullPath;

			int64 fileSize = 0;

			bool isOpen = false;

		} m_info;
	};
}

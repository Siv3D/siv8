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

# include <Siv3D/MemoryMappedFile.hpp>

namespace s3d
{
	class MemoryMappedFile::MemoryMappedFileDetail
	{
	public:

		[[nodiscard]]
		MemoryMappedFileDetail() = default;

		~MemoryMappedFileDetail();

		bool open(FilePathView path, OpenMode_if_Exists ifExists, OpenMode_if_NotFound ifNotFound);

		void close();

		[[nodiscard]]
		bool isOpen() const;

		[[nodiscard]]
		MappedMemory map(size_t offset, size_t requestSize);

		void unmap();

		bool flush() const;

		[[nodiscard]]
		int64 size() const;

		[[nodiscard]]
		const FilePath& path() const;

	private:

		//struct File
		//{
		//	HANDLE fileHandle = INVALID_HANDLE_VALUE;

		//	HANDLE fileMapping = nullptr;

		//	Byte* baseAddress	= nullptr;

		//} m_file;

		//struct Info
		//{
		//	FilePath fullPath;

		//	int64 fileSize = 0;

		//	bool isOpen = false;

		//} m_info;
	};
}

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

# include <Siv3D/MemoryMappedFileView.hpp>
# include <Siv3D/MemoryMappedFileView/MemoryMappedFileViewDetail.hpp>

namespace s3d
{
	MemoryMappedFileView::MemoryMappedFileView()
		: pImpl{ std::make_shared<MemoryMappedFileViewDetail>() } {}

	MemoryMappedFileView::MemoryMappedFileView(const FilePathView path)
		: MemoryMappedFileView{}
	{
		open(path);
	}

	bool MemoryMappedFileView::open(const FilePathView path)
	{
		return pImpl->open(path);
	}

	void MemoryMappedFileView::close()
	{
		pImpl->close();
	}

	bool MemoryMappedFileView::isOpen() const
	{
		return pImpl->isOpen();
	}

	MemoryMappedFileView::operator bool() const
	{
		return isOpen();
	}

	MappedMemory MemoryMappedFileView::map(const size_t offset, const size_t requestSize)
	{
		return pImpl->map(offset, requestSize);
	}

	MappedMemory MemoryMappedFileView::mapAll()
	{
		return pImpl->map(0, pImpl->size());
	}

	void MemoryMappedFileView::unmap()
	{
		pImpl->unmap();
	}

	int64 MemoryMappedFileView::size() const
	{
		return pImpl->size();
	}

	const FilePath& MemoryMappedFileView::path() const
	{
		return pImpl->path();
	}
}

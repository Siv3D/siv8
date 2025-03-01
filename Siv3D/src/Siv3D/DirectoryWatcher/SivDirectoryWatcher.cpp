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

# include <Siv3D/DirectoryWatcher.hpp>
# include <Siv3D/DirectoryWatcher/DirectoryWatcherDetail.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	DirectoryWatcher::DirectoryWatcher() {}

	DirectoryWatcher::DirectoryWatcher(const FilePathView directory, const Array<String>& applicableExtensions)
		: pImpl{ std::make_shared<DirectoryWatcherDetail>() }
	{
		if (not pImpl->start(directory, applicableExtensions))
		{
			pImpl.reset();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	DirectoryWatcher::~DirectoryWatcher() {}

	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	bool DirectoryWatcher::isOpen() const
	{
		if (not pImpl)
		{
			return false;
		}

		return pImpl->isActive();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	DirectoryWatcher::operator bool() const
	{
		return isOpen();
	}

	////////////////////////////////////////////////////////////////
	//
	//	retrieveChanges
	//
	////////////////////////////////////////////////////////////////

	Array<FileChange> DirectoryWatcher::retrieveChanges() const
	{
		Array<FileChange> changes;

		if (not retrieveChanges(changes))
		{
			return{};
		}

		return changes;
	}

	bool DirectoryWatcher::retrieveChanges(Array<FileChange>& fileChanges) const
	{
		if (not pImpl)
		{
			fileChanges.clear();
			
			return false;
		}

		pImpl->retrieveChanges(fileChanges);

		return (not fileChanges.isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearChanges
	//
	////////////////////////////////////////////////////////////////

	void DirectoryWatcher::clearChanges()
	{
		if (not pImpl)
		{
			return;
		}

		pImpl->clearChanges();
	}

	////////////////////////////////////////////////////////////////
	//
	//	directory
	//
	////////////////////////////////////////////////////////////////

	const FilePath& DirectoryWatcher::directory() const
	{
		if (not pImpl)
		{
			static const FilePath empty;
			return empty;
		}

		return pImpl->directory();
	}

	////////////////////////////////////////////////////////////////
	//
	//	applicableExtensions
	//
	////////////////////////////////////////////////////////////////

	const Array<String>& DirectoryWatcher::applicableExtensions() const
	{
		if (not pImpl)
		{
			static const Array<String> empty;
			return empty;
		}

		return pImpl->applicableExtensions();
	}
}

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

# include <Siv3D/EngineLog.hpp>
# include <Siv3D/SequenceFormatter.hpp>
# include <Siv3D/FileSystem.hpp>
# include "DirectoryWatcherDetail.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	DirectoryWatcher::DirectoryWatcherDetail::~DirectoryWatcherDetail()
	{


		LOG_INFO(fmt::format("ℹ️ DirectoryWatcher: Stopped watching `{}`", m_directory));
	}

	////////////////////////////////////////////////////////////////
	//
	//	start
	//
	////////////////////////////////////////////////////////////////

	bool DirectoryWatcher::DirectoryWatcherDetail::start(const FilePathView directory, const Array<String>& applicableExtensions)
	{
		if (directory.isEmpty()
			|| (not FileSystem::IsDirectory(directory)))
		{
			LOG_FAIL(fmt::format("❌ DirectoryWatcherDetail::start(): `{}` is not a directory", directory));
			return false;
		}

		m_directory = FileSystem::FullPath(directory);
		m_sortedApplicableExtensions = applicableExtensions.sorted_and_uniqued();

		return isActive();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isActive
	//
	////////////////////////////////////////////////////////////////

	bool DirectoryWatcher::DirectoryWatcherDetail::isActive() const
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	retrieveChanges
	//
	////////////////////////////////////////////////////////////////

	void DirectoryWatcher::DirectoryWatcherDetail::retrieveChanges(Array<FileChange>& fileChanges)
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	clearChanges
	//
	////////////////////////////////////////////////////////////////

	void DirectoryWatcher::DirectoryWatcherDetail::clearChanges()
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	directory
	//
	////////////////////////////////////////////////////////////////

	const FilePath& DirectoryWatcher::DirectoryWatcherDetail::directory() const noexcept
	{
		return m_directory;
	}

	////////////////////////////////////////////////////////////////
	//
	//	applicableExtensions
	//
	////////////////////////////////////////////////////////////////

	const Array<String>& DirectoryWatcher::DirectoryWatcherDetail::applicableExtensions() const
	{
		return m_sortedApplicableExtensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////
}

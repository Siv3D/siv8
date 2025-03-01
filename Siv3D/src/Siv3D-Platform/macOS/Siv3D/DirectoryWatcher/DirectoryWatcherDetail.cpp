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

# include <Siv3D/EngineLog.hpp>
# include <Siv3D/SequenceFormatter.hpp>
# include <Siv3D/FileSystem.hpp>
# include "DirectoryWatcherDetail.hpp"

namespace s3d
{
	namespace
	{
		static void Convert(CFStringRef str, char32_t* dst)
		{
			const size_t Length = CFStringGetLength(str);
			CFRange Range = CFRangeMake(0, Length);
			CFIndex usedBufLen;
			CFStringGetBytes(str, Range, kCFStringEncodingUTF32LE, '?', false, (uint8*)dst, 2048, &usedBufLen);
			dst[usedBufLen / sizeof(char32_t)] = 0;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	DirectoryWatcher::DirectoryWatcherDetail::~DirectoryWatcherDetail()
	{
		m_abort = true;
		
		if (m_eventStream == nullptr)
		{
			return;
		}
		
		FSEventStreamStop(m_eventStream);
		FSEventStreamUnscheduleFromRunLoop(m_eventStream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		FSEventStreamInvalidate(m_eventStream);
		FSEventStreamRelease(m_eventStream);
		m_eventStream = nullptr;
		
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
		m_extensionFilter.set(applicableExtensions);

		CFStringRef fullPath = CFStringCreateWithBytes(kCFAllocatorDefault,
													   reinterpret_cast<const uint8*>(m_directory.c_str()),
													   m_directory.size_bytes(), kCFStringEncodingUTF32LE, false);
		CFArrayRef pathToWatch = CFArrayCreate(NULL, (const void**)&fullPath, 1, NULL);
		
		FSEventStreamContext context = {
			.version			= 0,
			.info				= this,
			.retain				= NULL,
			.release			= NULL,
			.copyDescription	= NULL
		};
		
		m_eventStream = FSEventStreamCreate(NULL,
											&OnFileChange,
											&context,
											pathToWatch,
											kFSEventStreamEventIdSinceNow,
											LatencySec,
											(kFSEventStreamCreateFlagUseCFTypes | kFSEventStreamCreateFlagNoDefer | kFSEventStreamCreateFlagFileEvents)
											);
		
		CFRelease(pathToWatch);
		CFRelease(fullPath);
		
		if (not m_eventStream)
		{
			return false;
		}
		
		FSEventStreamScheduleWithRunLoop(m_eventStream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		FSEventStreamStart(m_eventStream);
		
		if (m_extensionFilter)
		{
			LOG_INFO(fmt::format("ℹ️ DirectoryWatcher: Started to watch `{}`. applicableExtensions = {}", m_directory, Format(m_extensionFilter.getSortedExtensions())));
		}
		else
		{
			LOG_INFO(fmt::format("ℹ️ DirectoryWatcher: Started to watch `{}`", m_directory));
		}
		
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isActive
	//
	////////////////////////////////////////////////////////////////

	bool DirectoryWatcher::DirectoryWatcherDetail::isActive() const
	{
		return (m_eventStream != nullptr);
	}

	////////////////////////////////////////////////////////////////
	//
	//	retrieveChanges
	//
	////////////////////////////////////////////////////////////////

	void DirectoryWatcher::DirectoryWatcherDetail::retrieveChanges(Array<FileChange>& fileChanges)
	{
		std::lock_guard lock{ m_fileChanges.mutex };

		fileChanges.assign(m_fileChanges.fileChanges.begin(), m_fileChanges.fileChanges.end());

		m_fileChanges.fileChanges.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearChanges
	//
	////////////////////////////////////////////////////////////////

	void DirectoryWatcher::DirectoryWatcherDetail::clearChanges()
	{
		std::lock_guard lock{ m_fileChanges.mutex };

		m_fileChanges.fileChanges.clear();
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
		return m_extensionFilter.getSortedExtensions();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void DirectoryWatcher::DirectoryWatcherDetail::onFileChange(const size_t eventCount, void* paths, const FSEventStreamEventFlags flags[])
	{
		if (m_abort)
		{
			return;
		}
		
		char32_t buffer[2048];
		CFArrayRef eventPathArray = (CFArrayRef)paths;
		
		std::lock_guard lock{ m_fileChanges.mutex };
		
		for (size_t i = 0; i < eventCount; ++i)
		{
			const FSEventStreamEventFlags flag = flags[i];
			const bool isFile = (flag & kFSEventStreamEventFlagItemIsFile);
			const bool isDirectory = (flag & kFSEventStreamEventFlagItemIsDir);
			
			if((not isFile) && (not isDirectory))
			{
				continue;
			}
			
			const bool renamed	= (flag & kFSEventStreamEventFlagItemRenamed);
			const bool modified	= (flag & kFSEventStreamEventFlagItemModified);
			const bool added	= (flag & kFSEventStreamEventFlagItemCreated);
			const bool removed	= (flag & kFSEventStreamEventFlagItemRemoved);
			bool checkExistance	= false;
			
			FileAction action = FileAction::Unknown;
			
			if (modified)
			{
				action = FileAction::Modified;
				checkExistance = true;
			}
			else if (renamed)
			{
				action = FileAction::Added;
				checkExistance = true;
			}
			else if (added)
			{
				action = FileAction::Added;
				
				if (removed)
				{
					checkExistance = true;
				}
			}
			else if (removed)
			{
				action = FileAction::Removed;
			}
			else
			{
				continue;
			}
			
			Convert((CFStringRef)CFArrayGetValueAtIndex(eventPathArray, i), buffer);
			
			FilePath fullPath{ buffer };
			
			if (isDirectory)
			{
				fullPath.push_back(U'/');
			}
			
			if (checkExistance && (not FileSystem::Exists(fullPath)))
			{
				action = FileAction::Removed;
			}
			
			if (isFile && m_extensionFilter)
			{
				if (m_extensionFilter.includes(FileSystem::Extension(fullPath)))
				{
					m_fileChanges.fileChanges.push_back({ std::move(fullPath), action });
				}
			}
			else
			{
				m_fileChanges.fileChanges.push_back({ std::move(fullPath), action });
			}
		}
	}

	void DirectoryWatcher::DirectoryWatcherDetail::OnFileChange(ConstFSEventStreamRef, void* pWatch, size_t eventCount, void* paths, const FSEventStreamEventFlags flags[], const FSEventStreamEventId[])
	{
		DirectoryWatcherDetail* watch = static_cast<DirectoryWatcherDetail*>(pWatch);
		watch->onFileChange(eventCount, paths, flags);
	}
}

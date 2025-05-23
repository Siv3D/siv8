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

# include <mutex>
# include <Siv3D/DirectoryWatcher.hpp>
# include <Siv3D/ExtensionFilter/ExtensionFilter.hpp>
# include <CoreServices/CoreServices.h>

namespace s3d
{
	class DirectoryWatcher::DirectoryWatcherDetail
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		DirectoryWatcherDetail() = default;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~DirectoryWatcherDetail();

		////////////////////////////////////////////////////////////////
		//
		//	start
		//
		////////////////////////////////////////////////////////////////

		bool start(const FilePathView directory, const Array<String>& applicableExtensions);

		////////////////////////////////////////////////////////////////
		//
		//	isActive
		//
		////////////////////////////////////////////////////////////////

		bool isActive() const;

		////////////////////////////////////////////////////////////////
		//
		//	retrieveChanges
		//
		////////////////////////////////////////////////////////////////

		void retrieveChanges(Array<FileChange>& fileChanges);

		////////////////////////////////////////////////////////////////
		//
		//	clearChanges
		//
		////////////////////////////////////////////////////////////////

		void clearChanges();

		////////////////////////////////////////////////////////////////
		//
		//	directory
		//
		////////////////////////////////////////////////////////////////

		const FilePath& directory() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	applicableExtensions
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const Array<String>& applicableExtensions() const;

	private:
		
		static constexpr double LatencySec = 0.25;

		FilePath m_directory;

		ExtensionFilter m_extensionFilter;

		FSEventStreamRef m_eventStream = nullptr;
		
		std::atomic<bool> m_abort = false;

		struct FileChanges
		{
			std::mutex mutex;

			Array<FileChange> fileChanges;
		
		} m_fileChanges;
		
		void onFileChange(const size_t eventCount, void* paths, const FSEventStreamEventFlags flags[]);
		
		static void OnFileChange(ConstFSEventStreamRef, void* pWatch, size_t eventCount, void* paths, const FSEventStreamEventFlags flags[], const FSEventStreamEventId[]);
	};
}

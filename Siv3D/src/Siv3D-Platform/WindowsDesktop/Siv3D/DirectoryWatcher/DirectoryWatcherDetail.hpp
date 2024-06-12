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

# include <thread>
# include <mutex>
# include <Siv3D/DirectoryWatcher.hpp>
# include <Siv3D/ExtensionFilter/ExtensionFilter.hpp>
# include <Siv3D/Windows/Windows.hpp>

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

		DirectoryWatcherDetail();

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

		static constexpr size_t BufferSize = (128 * 1024);

		static constexpr bool WatchSubtree = true;

		static constexpr uint32 NotificationFilter = (FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION);

		inline static decltype(ReadDirectoryChangesExW)* m_pReadDirectoryChangesExW;

		inline static bool m_ReadDirectoryChangesExW_initialized = false;

		FilePath m_directory;

		ExtensionFilter m_extensionFilter;

		std::jthread m_thread;

		std::atomic<bool> m_initCalled = false;

		HANDLE m_directoryHandle = INVALID_HANDLE_VALUE;

		Array<uint8> m_buffer;

		Array<uint8> m_backBuffer;

		OVERLAPPED m_overlapped{};

		struct FileChanges
		{
			std::mutex mutex;

			Array<FileChange> fileChanges;
		
		} m_fileChanges;

		bool init();

		void onChangeNotification(DWORD errorCode, DWORD numBytes);

		static void Run(std::stop_token stop_token, DirectoryWatcherDetail* watcher);

		static void OnChangeNotification(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, const LPOVERLAPPED lpOverlapped);
	};
}

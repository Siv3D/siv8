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

# include <Siv3D/DLL.hpp>
# include <Siv3D/SequenceFormatter.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/EngineLog.hpp>
# include "DirectoryWatcherDetail.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr FileAction ToFileAction(DWORD action) noexcept
		{
			switch (action)
			{
			case FILE_ACTION_ADDED:
			case FILE_ACTION_RENAMED_NEW_NAME:
				return FileAction::Added;
			case FILE_ACTION_MODIFIED:
				return FileAction::Modified;
			case FILE_ACTION_REMOVED:
			case FILE_ACTION_RENAMED_OLD_NAME:
				return FileAction::Removed;
			default:
				return FileAction::Unknown;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	DirectoryWatcher::DirectoryWatcherDetail::DirectoryWatcherDetail()
	{
		if (not m_ReadDirectoryChangesExW_initialized)
		{
			if (LibraryHandle kernel32 = DLL::LoadSystemLibraryNoThrow(L"kernel32.dll"))
			{
				m_pReadDirectoryChangesExW = DLL::GetFunctionNoThrow(kernel32, "ReadDirectoryChangesExW");
			}
			else
			{
				m_pReadDirectoryChangesExW = nullptr;
			}

			if (not m_pReadDirectoryChangesExW)
			{
				LOG_INFO("ℹ️ DirectoryWatcher: ReadDirectoryChangesExW is not available");
			}
			else
			{
				LOG_INFO("ℹ️ DirectoryWatcher: ReadDirectoryChangesExW is available");
			}

			m_ReadDirectoryChangesExW_initialized = true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	DirectoryWatcher::DirectoryWatcherDetail::~DirectoryWatcherDetail()
	{
		if (m_thread.joinable())
		{
			m_thread.request_stop();
			m_thread.join();
		}

		if (m_directoryHandle == INVALID_HANDLE_VALUE)
		{
			return;
		}

		::CancelIoEx(m_directoryHandle, &m_overlapped);
		HANDLE tempDirectoryHandle = std::exchange(m_directoryHandle, INVALID_HANDLE_VALUE);
		::WaitForSingleObjectEx(tempDirectoryHandle, 1000, true);
		::CloseHandle(tempDirectoryHandle);

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
		m_thread = std::jthread{ DirectoryWatcherDetail::Run, this };

		while (not m_initCalled)
		{
			::Sleep(1);
		}

		return isActive();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isActive
	//
	////////////////////////////////////////////////////////////////

	bool DirectoryWatcher::DirectoryWatcherDetail::isActive() const
	{
		return (m_directoryHandle != INVALID_HANDLE_VALUE);
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

	bool DirectoryWatcher::DirectoryWatcherDetail::init()
	{
		m_directoryHandle = ::CreateFileW(
			Unicode::ToWstring(m_directory).c_str(),
			FILE_LIST_DIRECTORY,
			(FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE),
			nullptr,
			OPEN_EXISTING,
			(FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED),
			nullptr
		);

		if (m_directoryHandle == INVALID_HANDLE_VALUE)
		{
			LOG_FAIL(fmt::format("❌ DirectoryWatcherDetail::start(): Failed to create a directory handle `{}`", m_directory));
			return false;
		}

		m_buffer.resize(BufferSize);
		m_backBuffer.resize(BufferSize);
		m_overlapped.hEvent = this;

		bool result = false;

		if (m_pReadDirectoryChangesExW)
		{
			result = static_cast<bool>(m_pReadDirectoryChangesExW(
				m_directoryHandle,
				m_buffer.data(),
				static_cast<DWORD>(m_buffer.size()),
				WatchSubtree,
				NotificationFilter,
				nullptr,
				&m_overlapped,
				&DirectoryWatcherDetail::OnChangeNotification,
				ReadDirectoryNotifyExtendedInformation));
		}
		else
		{
			result = static_cast<bool>(::ReadDirectoryChangesW(
				m_directoryHandle,
				m_buffer.data(),
				static_cast<DWORD>(m_buffer.size()),
				WatchSubtree,
				NotificationFilter,
				nullptr,
				&m_overlapped,
				&DirectoryWatcherDetail::OnChangeNotification));
		}

		if (not result)
		{
			::CloseHandle(m_directoryHandle);
			m_directoryHandle = INVALID_HANDLE_VALUE;

			LOG_FAIL(fmt::format("❌ DirectoryWatcher: ReadDirectoryChangesW() failed. `{}`", m_directory));
			return false;
		}

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

	void DirectoryWatcher::DirectoryWatcherDetail::onChangeNotification(const DWORD errorCode, const DWORD numBytes)
	{
		if ((errorCode == ERROR_OPERATION_ABORTED)
			|| (numBytes == 0))
		{
			return;
		}

		const bool isValid = (errorCode != ERROR_IO_INCOMPLETE);

		if (isValid)
		{
			m_backBuffer.swap(m_buffer);
		}

		bool result = false;

		if (m_pReadDirectoryChangesExW)
		{
			result = static_cast<bool>(m_pReadDirectoryChangesExW(
				m_directoryHandle,
				m_buffer.data(),
				static_cast<DWORD>(m_buffer.size()),
				WatchSubtree,
				NotificationFilter,
				nullptr,
				&m_overlapped,
				&DirectoryWatcherDetail::OnChangeNotification,
				ReadDirectoryNotifyExtendedInformation));
		}
		else
		{
			result = static_cast<bool>(::ReadDirectoryChangesW(
				m_directoryHandle,
				m_buffer.data(),
				static_cast<DWORD>(m_buffer.size()),
				WatchSubtree,
				NotificationFilter,
				nullptr,
				&m_overlapped,
				&DirectoryWatcherDetail::OnChangeNotification));
		}

		if (not result)
		{
			::CloseHandle(m_directoryHandle);
			m_directoryHandle = INVALID_HANDLE_VALUE;

			LOG_FAIL(fmt::format("❌ DirectoryWatcher: ReadDirectoryChangesW() failed. `{}`", m_directory));
			return;
		}

		if (not isValid)
		{
			return;
		}

		std::lock_guard lock{ m_fileChanges.mutex };

		if (m_pReadDirectoryChangesExW)
		{
			for (const uint8* pInfo = m_backBuffer.data();;)
			{
				const FILE_NOTIFY_EXTENDED_INFORMATION* notifyInfo = reinterpret_cast<const FILE_NOTIFY_EXTENDED_INFORMATION*>(pInfo);
				const std::wstring_view localPathView{ notifyInfo->FileName, (notifyInfo->FileNameLength / sizeof(WCHAR)) };
				const bool isDirectory = (notifyInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY);
				const String localPath = Unicode::FromWstring(localPathView).replace('\\', '/');
				const FilePath fullPath = (m_directory + localPath + (isDirectory ? U"/" : U""));
				const FileAction action = ToFileAction(notifyInfo->Action);

				if (m_extensionFilter) // 拡張子フィルタがある場合
				{
					if (m_extensionFilter.includes(FileSystem::Extension(localPath)))
					{
						m_fileChanges.fileChanges.emplace_back(fullPath, action);
					}
				}
				else
				{
					m_fileChanges.fileChanges.emplace_back(fullPath, action);
				}

				if (notifyInfo->NextEntryOffset == 0)
				{
					break;
				}

				pInfo += notifyInfo->NextEntryOffset;
			}
		}
		else
		{
			for (const uint8* pInfo = m_backBuffer.data();;)
			{
				const FILE_NOTIFY_INFORMATION* notifyInfo = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(pInfo);
				const std::wstring_view localPathView{ notifyInfo->FileName, (notifyInfo->FileNameLength / sizeof(WCHAR)) };
				const String localPath = Unicode::FromWstring(localPathView).replace('\\', '/');
				FilePath fullPath = (m_directory + localPath);
				
				if (FileSystem::IsDirectory(fullPath))
				{
					fullPath += U'/';
				}
		
				const FileAction action = ToFileAction(notifyInfo->Action);

				if (m_extensionFilter) // 拡張子フィルタがある場合
				{
					if (m_extensionFilter.includes(FileSystem::Extension(localPath)))
					{
						m_fileChanges.fileChanges.emplace_back(fullPath, action);
					}
				}
				else
				{
					m_fileChanges.fileChanges.emplace_back(fullPath, action);
				}

				if (notifyInfo->NextEntryOffset == 0)
				{
					break;
				}

				pInfo += notifyInfo->NextEntryOffset;
			}
		}
	}

	void DirectoryWatcher::DirectoryWatcherDetail::Run(std::stop_token stop_token, DirectoryWatcherDetail* watcher)
	{
		if (not watcher->init())
		{
			watcher->m_initCalled = true;
			return;
		}

		watcher->m_initCalled = true;

		while (not stop_token.stop_requested())
		{
			::SleepEx(15, true);
		}
	}

	void DirectoryWatcher::DirectoryWatcherDetail::OnChangeNotification(const DWORD dwErrorCode, const DWORD dwNumberOfBytesTransfered, const LPOVERLAPPED lpOverlapped)
	{
		DirectoryWatcherDetail* directoryWatcher = static_cast<DirectoryWatcherDetail*>(lpOverlapped->hEvent);
		directoryWatcher->onChangeNotification(dwErrorCode, dwNumberOfBytesTransfered);
	}
}

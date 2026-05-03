//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CDragDrop.hpp"
# include "GLFW_Siv3D_DragDropBridge.h"
# include <Siv3D/Time.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Scene.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CDragDrop::CDragDrop() = default;

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CDragDrop::~CDragDrop()
	{
		LOG_SCOPED_DEBUG("CDragDrop::~CDragDrop()");

		if (m_window)
		{
			GLFW_Siv3D_DragDropUnregister(m_window);
			m_window = nullptr;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::init()
	{
		LOG_SCOPED_DEBUG("CDragDrop::init()");

		m_window = SIV3D_ENGINE(Window)->getHandle();

		if (m_window)
		{
			GLFW_Siv3D_DragDropRegister(m_window, this);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::update()
	{
		// macOS では native callback が直接キューを更新するので no-op
	}

	////////////////////////////////////////////////////////////////
	//
	//	isAcceptingFilePaths
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::isAcceptingFilePaths() const
	{
		return m_acceptFilePaths.load(std::memory_order_relaxed);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isAcceptingText
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::isAcceptingText() const
	{
		return m_acceptText.load(std::memory_order_relaxed);
	}

	////////////////////////////////////////////////////////////////
	//
	//	acceptFilePaths
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::acceptFilePaths(const bool accept)
	{
		m_acceptFilePaths.store(accept, std::memory_order_relaxed);
	}

	////////////////////////////////////////////////////////////////
	//
	//	acceptText
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::acceptText(const bool accept)
	{
		m_acceptText.store(accept, std::memory_order_relaxed);
	}

	////////////////////////////////////////////////////////////////
	//
	//	dragOver
	//
	////////////////////////////////////////////////////////////////

	Optional<DragStatus> CDragDrop::dragOver() const
	{
		std::lock_guard lock{ m_stateMutex };

		if (not m_dragOver)
		{
			return none;
		}

		DragStatus status = *m_dragOver;
		status.cursorPos = Scene::ClientToScene(status.cursorPos).asPoint();
		return status;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNewFilePaths
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::hasNewFilePaths() const
	{
		std::lock_guard lock{ m_stateMutex };
		return (not m_droppedFilePaths.isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNewText
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::hasNewText() const
	{
		std::lock_guard lock{ m_stateMutex };
		return (not m_droppedTexts.isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::clear()
	{
		std::lock_guard lock{ m_stateMutex };
		m_droppedFilePaths.clear();
		m_droppedTexts.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	extractDroppedFilePaths
	//
	////////////////////////////////////////////////////////////////

	Array<DroppedFilePath> CDragDrop::extractDroppedFilePaths()
	{
		Array<DroppedFilePath> result;

		{
			std::lock_guard lock{ m_stateMutex };
			result = std::move(m_droppedFilePaths);
			m_droppedFilePaths.clear();
		}

		for (auto& item : result)
		{
			item.path = FileSystem::FullPath(item.path);
			item.pos = Scene::ClientToScene(item.pos).asPoint();
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	extractDroppedTexts
	//
	////////////////////////////////////////////////////////////////

	Array<DroppedText> CDragDrop::extractDroppedTexts()
	{
		Array<DroppedText> result;

		{
			std::lock_guard lock{ m_stateMutex };
			result = std::move(m_droppedTexts);
			m_droppedTexts.clear();
		}

		for (auto& item : result)
		{
			item.pos = Scene::ClientToScene(item.pos).asPoint();
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginDragFile
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::beginDragFile(const FilePathView path)
	{
		const FilePath fullPath = FileSystem::FullPath(path);

		if (fullPath.isEmpty() || (not FileSystem::IsFile(fullPath)))
		{
			return false;
		}

		std::lock_guard lock{ m_dragRequestMutex };

		if (m_dragInProgress.load(std::memory_order_relaxed) || m_dragRequest)
		{
			return false;
		}

		PendingDragRequest request;
		request.type = DragRequestType::FilePaths;
		request.filePaths = { fullPath };
		m_dragRequest = std::move(request);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginDragFiles
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::beginDragFiles(const Array<FilePath>& paths)
	{
		if (paths.isEmpty())
		{
			return false;
		}

		Array<FilePath> fullPaths;
		fullPaths.reserve(paths.size());

		for (const auto& path : paths)
		{
			const FilePath fullPath = FileSystem::FullPath(path);

			if (fullPath.isEmpty() || (not FileSystem::IsFile(fullPath)))
			{
				return false;
			}

			fullPaths.push_back(fullPath);
		}

		std::lock_guard lock{ m_dragRequestMutex };

		if (m_dragInProgress.load(std::memory_order_relaxed) || m_dragRequest)
		{
			return false;
		}

		PendingDragRequest request;
		request.type = DragRequestType::FilePaths;
		request.filePaths = std::move(fullPaths);
		m_dragRequest = std::move(request);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	beginDragText
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::beginDragText(const StringView text)
	{
		const String s{ text };

		if (s.isEmpty())
		{
			return false;
		}

		std::lock_guard lock{ m_dragRequestMutex };

		if (m_dragInProgress.load(std::memory_order_relaxed) || m_dragRequest)
		{
			return false;
		}

		PendingDragRequest request;
		request.type = DragRequestType::Text;
		request.text = s;
		m_dragRequest = std::move(request);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	acceptsFilePathsFromNative
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::acceptsFilePathsFromNative() const noexcept
	{
		return m_acceptFilePaths.load(std::memory_order_relaxed);
	}

	////////////////////////////////////////////////////////////////
	//
	//	acceptsTextFromNative
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::acceptsTextFromNative() const noexcept
	{
		return m_acceptText.load(std::memory_order_relaxed);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setDragOverFromNative
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::setDragOverFromNative(const Point& rawClientPos, const DragItemType itemType)
	{
		std::lock_guard lock{ m_stateMutex };

		m_dragOver = DragStatus
		{
			.cursorPos = rawClientPos,
			.itemType = itemType,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearDragOverFromNative
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::clearDragOverFromNative()
	{
		std::lock_guard lock{ m_stateMutex };
		m_dragOver.reset();
	}

	////////////////////////////////////////////////////////////////
	//
	//	appendDroppedFilePathsFromNative
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::appendDroppedFilePathsFromNative(const Array<FilePath>& paths, const Point& rawClientPos, const uint64 timeMillisec)
	{
		if (paths.isEmpty())
		{
			return;
		}

		Array<DroppedFilePath> items;
		items.reserve(paths.size());

		for (const auto& path : paths)
		{
			items.push_back(DroppedFilePath
			{
				.path = path,
				.pos = rawClientPos,
				.timeMillisec = timeMillisec,
			});
		}

		std::lock_guard lock{ m_stateMutex };

		m_dragOver.reset();

		m_droppedFilePaths.insert(m_droppedFilePaths.end(),
			std::make_move_iterator(items.begin()),
			std::make_move_iterator(items.end()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	appendDroppedTextFromNative
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::appendDroppedTextFromNative(const StringView text, const Point& rawClientPos, const uint64 timeMillisec)
	{
		const String s{ text };

		if (s.isEmpty())
		{
			return;
		}

		std::lock_guard lock{ m_stateMutex };

		m_dragOver.reset();

		m_droppedTexts.push_back(DroppedText
		{
			.text = s,
			.pos = rawClientPos,
			.timeMillisec = timeMillisec,
		});
	}

	////////////////////////////////////////////////////////////////
	//
	//	tryPopPendingDragRequest
	//
	////////////////////////////////////////////////////////////////

	bool CDragDrop::tryPopPendingDragRequest(PendingDragRequest& request)
	{
		std::lock_guard lock{ m_dragRequestMutex };

		if (m_dragInProgress.load(std::memory_order_relaxed) || (not m_dragRequest))
		{
			return false;
		}

		request = std::move(*m_dragRequest);
		m_dragRequest.reset();
		m_dragInProgress.store(true, std::memory_order_relaxed);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	sourceEndedFromNative
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::sourceEndedFromNative() noexcept
	{
		m_dragInProgress.store(false, std::memory_order_relaxed);
	}
}

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
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CDragDrop::init()
	{
		LOG_SCOPED_DEBUG("CDragDrop::init()");
	}

	void CDragDrop::update()
	{
		if (not m_pDropTargetState)
		{
			return;
		}
		m_dragOver = std::move(snapshot.dragOver);
	}

	void CDragDrop::acceptFilePaths(const bool accept)
	{

	}

	void CDragDrop::acceptText(const bool accept)
	{

	}

	Optional<DragStatus> CDragDrop::dragOver() const
	{
		return(none);
	}

	bool CDragDrop::hasNewFilePaths() const
	{
		return(false);
	}

	bool CDragDrop::hasNewText() const
	{
		return(false);
	}

	void CDragDrop::clear()
	{
		m_droppedFilePaths.clear();
		m_droppedTexts.clear();
	}

	Array<DroppedFilePath> CDragDrop::extractDroppedFilePaths()
	{
		return{};
	}

	Array<DroppedText> CDragDrop::extractDroppedTexts()
	{
		return{};
	}

	bool CDragDrop::beginDragFile(const FilePathView path)
	{
		return(false);
	}

	bool CDragDrop::beginDragFiles(const Array<FilePath>& paths)
	{
		return(false);
	}

	bool CDragDrop::beginDragText(const StringView text)
	{
		return(false);
	}
}

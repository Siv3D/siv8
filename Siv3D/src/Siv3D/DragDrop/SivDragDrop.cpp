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

# include <Siv3D/DragDrop.hpp>
# include <Siv3D/DragDrop/IDragDrop.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace DragDrop
	{
		void AcceptFilePaths(const bool accept)
		{
			SIV3D_ENGINE(DragDrop)->acceptFilePaths(accept);
		}

		void AcceptText(const bool accept)
		{
			SIV3D_ENGINE(DragDrop)->acceptText(accept);
		}

		Optional<DragStatus> DragOver()
		{
			return SIV3D_ENGINE(DragDrop)->dragOver();
		}

		bool HasDroppedFilePaths()
		{
			return SIV3D_ENGINE(DragDrop)->hasNewFilePaths();
		}

		bool HasDroppedText()
		{
			return SIV3D_ENGINE(DragDrop)->hasNewText();
		}

		void Clear()
		{
			SIV3D_ENGINE(DragDrop)->clear();
		}

		Array<DroppedFilePath> ExtractDroppedFilePaths()
		{
			return SIV3D_ENGINE(DragDrop)->extractDroppedFilePaths();
		}

		Array<DroppedText> ExtractDroppedTexts()
		{
			return SIV3D_ENGINE(DragDrop)->extractDroppedTexts();
		}

		bool BeginDragFile(const FilePathView path)
		{
			return SIV3D_ENGINE(DragDrop)->beginDragFile(path);
		}

		bool BeginDragFiles(const Array<FilePath>& paths)
		{
			return SIV3D_ENGINE(DragDrop)->beginDragFiles(paths);
		}

		bool BeginDragText(const StringView text)
		{
			return SIV3D_ENGINE(DragDrop)->beginDragText(text);
		}
	}
}

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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Optional.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/DragStatus.hpp>
# include <Siv3D/DroppedFilePath.hpp>
# include <Siv3D/DroppedText.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DDragDrop
	{
	public:

		[[nodiscard]]
		static ISiv3DDragDrop* Create();

		virtual ~ISiv3DDragDrop() = default;

		virtual void init() = 0;

		virtual void update() = 0;

		virtual void acceptFilePaths(bool accept) = 0;

		virtual void acceptText(bool accept) = 0;

		[[nodiscard]]
		virtual Optional<DragStatus> dragOver() const = 0;

		[[nodiscard]]
		virtual bool hasNewFilePaths() const = 0;

		[[nodiscard]]
		virtual bool hasNewText() const = 0;

		virtual void clear() = 0;

		[[nodiscard]]
		virtual Array<DroppedFilePath> extractDroppedFilePaths() = 0;

		[[nodiscard]]
		virtual Array<DroppedText> extractDroppedTexts() = 0;

		[[nodiscard]]
		virtual bool beginDragFile(FilePathView path) = 0;

		[[nodiscard]]
		virtual bool beginDragFiles(const Array<FilePath>& paths) = 0;

		[[nodiscard]]
		virtual bool beginDragText(StringView text) = 0;
	};
}

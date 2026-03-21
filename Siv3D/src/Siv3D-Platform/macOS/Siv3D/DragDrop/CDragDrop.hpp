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
# include <mutex>
# include <Siv3D/DragDrop/IDragDrop.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Windows/ComPtr.hpp>

namespace s3d
{
	namespace detail
	{
		class DropTargetState;
	}

	class CDragDrop final : public ISiv3DDragDrop
	{
	public:

		CDragDrop();

		~CDragDrop() override;

		void init() override;

		void update() override;

		void acceptFilePaths(bool accept) override;

		void acceptText(bool accept) override;

		[[nodiscard]]
		Optional<DragStatus> dragOver() const override;

		[[nodiscard]]
		bool hasNewFilePaths() const override;

		[[nodiscard]]
		bool hasNewText() const override;

		void clear() override;

		[[nodiscard]]
		Array<DroppedFilePath> extractDroppedFilePaths() override;

		[[nodiscard]]
		Array<DroppedText> extractDroppedTexts() override;

		[[nodiscard]]
		bool beginDragFile(FilePathView path) override;

		[[nodiscard]]
		bool beginDragFiles(const Array<FilePath>& paths) override;

		[[nodiscard]]
		bool beginDragText(StringView text) override;

	private:

	};
}

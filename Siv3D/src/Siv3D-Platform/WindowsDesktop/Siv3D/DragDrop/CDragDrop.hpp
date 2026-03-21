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
		
		[[nodiscard]]
		bool isAcceptingFilePaths() const override;

		[[nodiscard]]
		bool isAcceptingText() const override;

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

		// WindowProc 側（OleInitialize 済み / メッセージループ側）から呼ぶ
		void process();

	private:

		enum class DragRequestType : uint8
		{
			FilePaths,
			Text,
		};

		struct DragRequest
		{
			DragRequestType type = DragRequestType::FilePaths;
			Array<FilePath> filePaths;
			String text;
		};

		HWND m_hWnd = nullptr;

		ComPtr<IDropTarget> m_pDropTarget;

		std::shared_ptr<detail::DropTargetState> m_pDropTargetState;

		bool m_registered = false;

		std::atomic_bool m_acceptFilePaths{ true };

		std::atomic_bool m_acceptText{ false };

		Array<DroppedFilePath> m_droppedFilePaths;

		Array<DroppedText> m_droppedTexts;

		Optional<DragStatus> m_dragOver;

		mutable std::mutex m_dragRequestMutex;

		Optional<DragRequest> m_dragRequest;

		std::atomic_bool m_dragInProgress{ false };
	};
}

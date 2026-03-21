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
# include <atomic>
# include <mutex>
# include <Siv3D/DragDrop/IDragDrop.hpp>

namespace s3d
{
	class CDragDrop final : public ISiv3DDragDrop
	{
	public:

		enum class DragRequestType : uint8
		{
			FilePaths,
			Text,
		};

		struct PendingDragRequest
		{
			DragRequestType type = DragRequestType::FilePaths;
			Array<FilePath> filePaths;
			String text;
		};

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

		// native bridge 用
		[[nodiscard]]
		bool acceptsFilePathsFromNative() const noexcept;

		[[nodiscard]]
		bool acceptsTextFromNative() const noexcept;

		void setDragOverFromNative(const Point& rawClientPos, DragItemType itemType);

		void clearDragOverFromNative();

		void appendDroppedFilePathsFromNative(const Array<FilePath>& paths, const Point& rawClientPos, uint64 timeMillisec);

		void appendDroppedTextFromNative(StringView text, const Point& rawClientPos, uint64 timeMillisec);

		[[nodiscard]]
		bool tryPopPendingDragRequest(PendingDragRequest& request);

		void sourceEndedFromNative() noexcept;

	private:

		void* m_window = nullptr;

		std::atomic_bool m_acceptFilePaths{ true };

		std::atomic_bool m_acceptText{ false };

		mutable std::mutex m_stateMutex;

		Array<DroppedFilePath> m_droppedFilePaths;

		Array<DroppedText> m_droppedTexts;

		Optional<DragStatus> m_dragOver;

		mutable std::mutex m_dragRequestMutex;

		Optional<PendingDragRequest> m_dragRequest;

		std::atomic_bool m_dragInProgress{ false };
	};
}
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
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Clipboard/IClipboard.hpp>

namespace s3d
{
	class CClipboard final : public ISiv3DClipboard
	{
	public:

		CClipboard() = default;

		~CClipboard() override;

		void init() override;

		bool hasChanged() override;

		uint64 getSequenceNumber() override;

		void clear() override;

		void setText(StringView text) override;

		bool getText(String& text) override;

		bool hasText() override;

		void setImage(const Image& image, bool usePNG) override;

		bool getImage(Image& image) override;

		bool hasImage() override;

		void setFilePaths(const Array<FilePath>& paths) override;

		bool getFilePaths(Array<FilePath>& paths) override;

		void setHTML(StringView html) override;

		bool getHTML(String& html) override;

		void setData(StringView mimeType, const void* data, size_t size) override;

		bool getData(StringView mimeType, Blob& data) override;

		Array<String> getAvailableMimeTypes() override;

	private:

		HWND m_hWnd = nullptr;

		uint64 m_sequenceNumber = 0;
	};
}

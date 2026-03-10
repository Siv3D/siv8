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
# include "PDFiumAPI.hpp"

namespace s3d
{
	struct PDFPage;
	struct PDFRenderOptions;

	class PDFDocumentData
	{
	public:

		struct Null {};

		PDFDocumentData(Null);

		PDFDocumentData(FilePathView path, StringView password, const PDFiumAPI* api);

		~PDFDocumentData();

		[[nodiscard]]
		bool isInitialized() const noexcept;

		[[nodiscard]]
		String getInfo() const;

		[[nodiscard]]
		bool isOpen() const noexcept;

		[[nodiscard]]
		int32 getPageCount() const;

		[[nodiscard]]
		Size getPageSize(int32 pageIndex, double scale) const;

		[[nodiscard]]
		bool renderPage(int32 pageIndex, PDFPage& result, const PDFRenderOptions& options) const;

	private:

		bool m_initialized = false;

		const PDFiumAPI* m_api = nullptr;

		struct Document
		{
			FPDF_DOCUMENT data = nullptr;

			int32 pageCount = 0;

			[[nodiscard]]
			explicit operator bool() const noexcept;

			[[nodiscard]]
			String getInfo() const;
		
		} m_document;
	};
}

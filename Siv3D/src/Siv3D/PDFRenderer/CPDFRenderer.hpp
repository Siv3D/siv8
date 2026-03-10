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
# include "IPDFRenderer.hpp"
# include "PDFiumAPI.hpp"
# include <Siv3D/Font.hpp>
# include <Siv3D/AssetHandleManager/AssetHandleManager.hpp>
# include "PDFDocumentData.hpp"

namespace s3d
{
	class CPDFRenderer final : public ISiv3DPDFRenderer
	{
	public:

		~CPDFRenderer() override;

		void init() override;

		PDFDocument::IDType createFromFile(FilePathView path) override;

		void release(PDFDocument::IDType handleID) override;

		int32 getPageCount(PDFDocument::IDType handleID) override;

		bool renderPage(PDFDocument::IDType handleID, int32 pageIndex, PDFPage& result, const PDFRenderOptions& options) override;

	private:

		enum class State
		{
			Uninitialized,	// PDFiumAPI のロード前
			Ready,			// PDFiumAPI のロード成功
			Failed,			// PDFiumAPI のロード失敗
		} m_state = State::Uninitialized;

		LibraryHandle m_library = nullptr;

		PDFiumAPI m_api;

		AssetHandleManager<PDFDocument::IDType, PDFDocumentData> m_documents{ "PDFDocument" };

		void setup();
	};
}

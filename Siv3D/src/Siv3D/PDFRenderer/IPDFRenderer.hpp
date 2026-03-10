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
# include <Siv3D/PDFDocument.hpp>

namespace s3d
{
	struct PDFPage;
	struct PDFRenderOptions;

	class SIV3D_NOVTABLE ISiv3DPDFRenderer
	{
	public:

		[[nodiscard]]
		static ISiv3DPDFRenderer* Create();

		virtual ~ISiv3DPDFRenderer() = default;

		virtual void init() = 0;

		virtual PDFDocument::IDType createFromFile(FilePathView path) = 0;

		virtual void release(PDFDocument::IDType handleID) = 0;

		virtual int32 getPageCount(PDFDocument::IDType handleID) = 0;

		virtual bool renderPage(PDFDocument::IDType handleID, int32 pageIndex, PDFPage& result, const PDFRenderOptions& options) = 0;
	};
}

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
# include <Siv3D/DLL.hpp>
# include <ThirdParty-prebuilt/PDFium/fpdfview.h>
# include <ThirdParty-prebuilt/PDFium/fpdf_doc.h>

namespace s3d
{
	struct PDFiumAPI
	{
		decltype(::FPDF_InitLibrary)* FPDF_InitLibrary = nullptr;
		decltype(::FPDF_DestroyLibrary)* FPDF_DestroyLibrary = nullptr;
		decltype(::FPDF_LoadDocument)* FPDF_LoadDocument = nullptr;
		decltype(::FPDF_GetLastError)* FPDF_GetLastError = nullptr;
		decltype(::FPDF_GetPageCount)* FPDF_GetPageCount = nullptr;
		decltype(::FPDF_LoadPage)* FPDF_LoadPage = nullptr;
		decltype(::FPDF_GetPageWidthF)* FPDF_GetPageWidthF = nullptr;
		decltype(::FPDF_GetPageHeightF)* FPDF_GetPageHeightF = nullptr;
		decltype(::FPDFBitmap_Create)* FPDFBitmap_Create = nullptr;
		decltype(::FPDFBitmap_FillRect)* FPDFBitmap_FillRect = nullptr;
		decltype(::FPDF_RenderPageBitmap)* FPDF_RenderPageBitmap = nullptr;
		decltype(::FPDFBitmap_GetBuffer)* FPDFBitmap_GetBuffer = nullptr;
		decltype(::FPDFBitmap_GetStride)* FPDFBitmap_GetStride = nullptr;
		decltype(::FPDFBitmap_Destroy)* FPDFBitmap_Destroy = nullptr;
		decltype(::FPDF_ClosePage)* FPDF_ClosePage = nullptr;
		decltype(::FPDF_CloseDocument)* FPDF_CloseDocument = nullptr;

		decltype(::FPDFLink_Enumerate)* FPDFLink_Enumerate = nullptr;
		decltype(::FPDFLink_GetAnnotRect)* FPDFLink_GetAnnotRect = nullptr;
		decltype(::FPDFLink_CountQuadPoints)* FPDFLink_CountQuadPoints = nullptr;
		decltype(::FPDFLink_GetQuadPoints)* FPDFLink_GetQuadPoints = nullptr;
		decltype(::FPDFLink_GetAction)* FPDFLink_GetAction = nullptr;
		decltype(::FPDFLink_GetDest)* FPDFLink_GetDest = nullptr;

		decltype(::FPDFAction_GetType)* FPDFAction_GetType = nullptr;
		decltype(::FPDFAction_GetDest)* FPDFAction_GetDest = nullptr;
		decltype(::FPDFAction_GetURIPath)* FPDFAction_GetURIPath = nullptr;
		decltype(::FPDFDest_GetDestPageIndex)* FPDFDest_GetDestPageIndex = nullptr;

		bool init(LibraryHandle lib);
	};
}

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

# include <Siv3D/Error.hpp>
# include "PDFiumAPI.hpp"

namespace s3d
{
	bool PDFiumAPI::init(LibraryHandle lib)
	{
		try
		{
			FPDF_InitLibrary			= DLL::GetFunction(lib, "FPDF_InitLibrary");
			FPDF_DestroyLibrary			= DLL::GetFunction(lib, "FPDF_DestroyLibrary");
			FPDF_LoadDocument			= DLL::GetFunction(lib, "FPDF_LoadDocument");
			FPDF_GetLastError			= DLL::GetFunction(lib, "FPDF_GetLastError");
			FPDF_GetPageCount			= DLL::GetFunction(lib, "FPDF_GetPageCount");
			FPDF_LoadPage				= DLL::GetFunction(lib, "FPDF_LoadPage");
			FPDF_GetPageWidthF			= DLL::GetFunction(lib, "FPDF_GetPageWidthF");
			FPDF_GetPageHeightF			= DLL::GetFunction(lib, "FPDF_GetPageHeightF");
			FPDF_GetPageSizeByIndexF	= DLL::GetFunction(lib, "FPDF_GetPageSizeByIndexF");
			FPDFBitmap_Create			= DLL::GetFunction(lib, "FPDFBitmap_Create");
			FPDFBitmap_FillRect			= DLL::GetFunction(lib, "FPDFBitmap_FillRect");
			FPDF_RenderPageBitmap		= DLL::GetFunction(lib, "FPDF_RenderPageBitmap");
			FPDFBitmap_GetBuffer		= DLL::GetFunction(lib, "FPDFBitmap_GetBuffer");
			FPDFBitmap_GetStride		= DLL::GetFunction(lib, "FPDFBitmap_GetStride");
			FPDFBitmap_Destroy			= DLL::GetFunction(lib, "FPDFBitmap_Destroy");
			FPDF_ClosePage				= DLL::GetFunction(lib, "FPDF_ClosePage");
			FPDF_CloseDocument			= DLL::GetFunction(lib, "FPDF_CloseDocument");

			FPDFLink_Enumerate			= DLL::GetFunction(lib, "FPDFLink_Enumerate");
			FPDFLink_GetAnnotRect		= DLL::GetFunction(lib, "FPDFLink_GetAnnotRect");
			FPDFLink_CountQuadPoints	= DLL::GetFunction(lib, "FPDFLink_CountQuadPoints");
			FPDFLink_GetQuadPoints		= DLL::GetFunction(lib, "FPDFLink_GetQuadPoints");
			FPDFLink_GetAction			= DLL::GetFunction(lib, "FPDFLink_GetAction");
			FPDFLink_GetDest			= DLL::GetFunction(lib, "FPDFLink_GetDest");

			FPDFAction_GetType			= DLL::GetFunction(lib, "FPDFAction_GetType");
			FPDFAction_GetDest			= DLL::GetFunction(lib, "FPDFAction_GetDest");
			FPDFAction_GetURIPath		= DLL::GetFunction(lib, "FPDFAction_GetURIPath");
			FPDFDest_GetDestPageIndex	= DLL::GetFunction(lib, "FPDFDest_GetDestPageIndex");
		}
		catch (const Error&)
		{
			return false;
		}

		return true;
	}
}

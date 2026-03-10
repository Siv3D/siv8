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

# include <Siv3D/Unicode.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/Math.hpp>
# include <Siv3D/PDFPage.hpp>
# include <Siv3D/PDFRenderOptions.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/EngineLog.hpp>
# include "PDFDocumentData.hpp"

namespace s3d
{
	PDFDocumentData::PDFDocumentData(Null)
		: m_initialized{ true } {}

	PDFDocumentData::PDFDocumentData(const FilePathView path, const StringView password, const PDFiumAPI* api)
	{
		m_api = api;

		const std::string pathUTF8 = Unicode::ToUTF8(path);
		FPDF_DOCUMENT document = nullptr;

		if (password)
		{
			document = m_api->FPDF_LoadDocument(pathUTF8.c_str(), Unicode::ToUTF8(password).c_str());
		}
		else
		{
			document = m_api->FPDF_LoadDocument(pathUTF8.c_str(), nullptr);
		}

		if (not document)
		{
			const auto error = m_api->FPDF_GetLastError();

			switch (error)
			{
			default:
			case 1: // unknown error
				{
					LOG_FAIL(U"Unknown error while opening PDF document: `{}` (error code: {})"_fmt(path, error));
					break;
				}
			case 2: // file not found or could not be opened
				{
					LOG_FAIL(U"File not found or could not be opened: `{}`"_fmt(path));
					break;
				}
			case 3: // file not in PDF format or corrupted
				{
					LOG_FAIL(U"File not in PDF format or corrupted: `{}`"_fmt(path));
					break;
				}
			case 4: // password required or incorrect password
				{
					if (password)
					{
						LOG_FAIL(U"Incorrect password for PDF document: `{}`"_fmt(path));
					}
					else
					{
						LOG_FAIL(U"Password required to open PDF document: `{}`"_fmt(path));
					}

					break;
				}
			case 5: // unsupported security scheme
				{
					LOG_FAIL(U"Unsupported security scheme for PDF document: `{}`"_fmt(path));
					break;
				}
			case 6: // page not found or content error
				{
					LOG_FAIL(U"Page not found or content error in PDF document: `{}`"_fmt(path));
					break;
				}
			}

			return;
		}

		m_document.data = document;
		m_document.pageCount = m_api->FPDF_GetPageCount(document);

		m_initialized = true;
	}

	PDFDocumentData::~PDFDocumentData()
	{
		if (m_api && m_document)
		{
			m_api->FPDF_CloseDocument(m_document.data);
			m_document = {};
			m_api = nullptr;
			m_initialized = false;
		}
	}

	bool PDFDocumentData::isInitialized() const noexcept
	{
		return m_initialized;
	}

	String PDFDocumentData::getInfo() const
	{
		return m_document.getInfo();
	}

	bool PDFDocumentData::isOpen() const noexcept
	{
		return static_cast<bool>(m_document);
	}

	int32 PDFDocumentData::getPageCount() const
	{
		return m_document.pageCount;
	}

	PDFDocumentData::Document::operator bool() const noexcept
	{
		return (data != nullptr);
	}

	String PDFDocumentData::Document::getInfo() const
	{
		return U"(pageCount = {})"_fmt(pageCount);
	}

	bool PDFDocumentData::renderPage(const int32 pageIndex, PDFPage& result, const PDFRenderOptions& options)
	{
		result.image.clear();
		result.scale = Max(0.0, options.scale);
		result.rotation = (InRange(options.rotation, 0, 3) ? options.rotation : 0);

		if (not isOpen())
		{
			return false;
		}

		if ((pageIndex < 0) || (m_document.pageCount <= pageIndex))
		{
			return false;
		}

		const FPDF_PAGE page = m_api->FPDF_LoadPage(m_document.data, pageIndex);
		if (not page)
		{
			return false;
		}
		const ScopeExit closePage{ [&]() { m_api->FPDF_ClosePage(page); } };

		const double pageWidthPt = m_api->FPDF_GetPageWidthF(page);
		const double pageHeightPt = m_api->FPDF_GetPageHeightF(page);
		
		const int32 width = Clamp(static_cast<int32>(Math::Round(pageWidthPt * options.scale)), 1, Image::MaxWidth);
		const int32 height = Clamp(static_cast<int32>(Math::Round(pageHeightPt * options.scale)), 1, Image::MaxHeight);

		const FPDF_BITMAP bitmap = m_api->FPDFBitmap_Create(width, height, 1);
		if (not bitmap)
		{
			return false;
		}
		const ScopeExit destroyBitmap{ [&]() { m_api->FPDFBitmap_Destroy(bitmap); } };

		const Color backgroundColor = options.backgroundColor;
		const uint32 bg =
			(static_cast<uint32>(backgroundColor.a) << 24) |
			(static_cast<uint32>(backgroundColor.r) << 16) |
			(static_cast<uint32>(backgroundColor.g) << 8) |
			(static_cast<uint32>(backgroundColor.b));
		m_api->FPDFBitmap_FillRect(bitmap, 0, 0, width, height, bg);

		int32 flags = 0;
		if (options.renderAnnotations)
		{
			flags |= FPDF_ANNOT;
		}

		m_api->FPDF_RenderPageBitmap(
			bitmap,
			page,
			0, 0,
			width, height,
			options.rotation,
			flags
		);

		const uint8* pSrc = static_cast<const uint8*>(m_api->FPDFBitmap_GetBuffer(bitmap));
		const int32 stride = m_api->FPDFBitmap_GetStride(bitmap);

		if ((not pSrc) || (stride <= 0))
		{
			return false;
		}

		result.image.resize(width, height);

		Color* pDst = result.image.data();

		for (int32 y = 0; y < height; ++y)
		{
			const uint8* pSrcLine = pSrc;

			for (int32 x = 0; x < width; ++x)
			{
				pDst->b = (*pSrcLine++);
				pDst->g = (*pSrcLine++);
				pDst->r = (*pSrcLine++);
				pDst->a = (*pSrcLine++);
				++pDst;
			}

			pSrc += stride;
		}

		return true;


		//RenderResult result;
		//result.image = std::move(image);
		//result.pageSizePt = Size{
		//	static_cast<int32>(Math::Round(pageWidthPt)),
		//	static_cast<int32>(Math::Round(pageHeightPt))
		//};
		//result.scale = options.scale;
		//result.rotation = options.rotation;
		//result.links = collectLinks(page, pageHeightPt, options.scale);

		//return result;
	}
}

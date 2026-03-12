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
	namespace
	{
		static void LogPDFiumError(const PDFiumAPI* api, const bool hasPasword)
		{
			const int32 errorCode = api->FPDF_GetLastError();

			switch (errorCode)
			{
				default:
				case 1: // unknown error
				{
					LOG_FAIL(fmt::format("Unknown error while opening PDF document: (error code: {})", errorCode));
					break;
				}
				case 2: // file not found or could not be opened
				{
					LOG_FAIL("File not found or could not be opened");
					break;
				}
				case 3: // file not in PDF format or corrupted
				{
					LOG_FAIL("File not in PDF format or corrupted");
					break;
				}
				case 4: // password required or incorrect password
				{
					if (hasPasword)
					{
						LOG_FAIL("Incorrect password for PDF document");
					}
					else
					{
						LOG_FAIL("Password required to open PDF document");
					}

					break;
				}
				case 5: // unsupported security scheme
				{
					LOG_FAIL("Unsupported security scheme for PDF document");
					break;
				}
				case 6: // page not found or content error
				{
					LOG_FAIL("Page not found or content error in PDF document");
					break;
				}
			}
		}

		[[nodiscard]]
		static String GetVersion(const PDFiumAPI* api, const FPDF_DOCUMENT document)
		{
			int32 version = 0;
			api->FPDF_GetFileVersion(document, &version);
			const int32 major = (version / 10);
			const int32 minor = (version % 10);
			return U"{}.{}"_fmt(major, minor);
		}

		[[nodiscard]]
		static bool RenderImage(const PDFiumAPI* api, const FPDF_PAGE page, const PDFRenderOptions& options, PDFPage& result)
		{
			const double pageWidthPt = api->FPDF_GetPageWidthF(page);
			const double pageHeightPt = api->FPDF_GetPageHeightF(page);

			const int32 width = Clamp(static_cast<int32>(Math::Round(pageWidthPt * options.scale)), 1, Image::MaxWidth);
			const int32 height = Clamp(static_cast<int32>(Math::Round(pageHeightPt * options.scale)), 1, Image::MaxHeight);

			const FPDF_BITMAP bitmap = api->FPDFBitmap_Create(width, height, 1);
			if (not bitmap)
			{
				return false;
			}
			const ScopeExit destroyBitmap{ [&]() { api->FPDFBitmap_Destroy(bitmap); } };

			const Color backgroundColor = options.backgroundColor;
			const uint32 bg =
				(static_cast<uint32>(backgroundColor.a) << 24) |
				(static_cast<uint32>(backgroundColor.r) << 16) |
				(static_cast<uint32>(backgroundColor.g) << 8) |
				(static_cast<uint32>(backgroundColor.b));
			api->FPDFBitmap_FillRect(bitmap, 0, 0, width, height, bg);

			int32 flags = 0;
			if (options.renderAnnotations)
			{
				flags |= FPDF_ANNOT;
			}

			api->FPDF_RenderPageBitmap(
				bitmap,
				page,
				0, 0,
				width, height,
				options.rotation,
				flags
			);

			const uint8* pSrc = static_cast<const uint8*>(api->FPDFBitmap_GetBuffer(bitmap));
			const int32 stride = api->FPDFBitmap_GetStride(bitmap);

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
		}

		[[nodiscard]]
		static RectF ToRectF(const FS_RECTF& rect, const double pageHeightPt, const double scale) noexcept
		{
			const double x = (rect.left * scale);
			const double y = ((pageHeightPt - rect.top) * scale);
			const double w = ((rect.right - rect.left) * scale);
			const double h = ((rect.top - rect.bottom) * scale);
			return{ x, y, w, h };
		}

		[[nodiscard]]
		static Array<PDFLinkInfo> CollectLinks(const PDFiumAPI* api, const FPDF_DOCUMENT document, const FPDF_PAGE page, const int32 pageCount, const double pageHeightPt, const double scale)
		{
			Array<PDFLinkInfo> links;

			int32 linkIndex = 0;
			FPDF_LINK link = nullptr;

			while (api->FPDFLink_Enumerate(page, &linkIndex, &link))
			{
				PDFLinkInfo linkInfo;

				if (FS_RECTF rect{};
					api->FPDFLink_GetAnnotRect(link, &rect))
				{
					const RectF region = ToRectF(rect, pageHeightPt, scale);
					linkInfo.region = region;
				}

				//if (const int32 quadCount = api->FPDFLink_CountQuadPoints(link))
				//{
				//	
				//}

				std::string uriBuffer;

				if (const FPDF_ACTION action = api->FPDFLink_GetAction(link))
				{
					const auto actionType = api->FPDFAction_GetType(action);

					if (actionType == PDFACTION_URI)
					{
						if (const auto size = api->FPDFAction_GetURIPath(document, action, nullptr, 0);
							0 < size)
						{
							uriBuffer.resize(size);
							const size_t written = api->FPDFAction_GetURIPath(document, action, uriBuffer.data(), size);

							if (0 < written)
							{
								if ((not uriBuffer.empty()) && (uriBuffer.back() == '\0'))
								{
									uriBuffer.pop_back();
								}

								linkInfo.target = Unicode::FromUTF8(uriBuffer);
							}
						}
					}
					else if (actionType == PDFACTION_GOTO)
					{
						if (FPDF_DEST dest = api->FPDFAction_GetDest(document, action))
						{
							if (const int32 destPage = api->FPDFDest_GetDestPageIndex(document, dest);
								(0 <= destPage) && (destPage < pageCount))
							{
								linkInfo.target = destPage;
							}
						}
					}
				}
				else if (const FPDF_DEST dest = api->FPDFLink_GetDest(document, link))
				{
					if (const int32 destPage = api->FPDFDest_GetDestPageIndex(document, dest);
						(0 <= destPage) && (destPage < pageCount))
					{
						linkInfo.target = destPage;
					}
				}

				if (linkInfo)
				{
					links.push_back(std::move(linkInfo));
				}
			}

			return links;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

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
			LogPDFiumError(m_api, static_cast<bool>(password));
			return;
		}

		m_document.data = document;
		m_document.version = GetVersion(m_api, document);
		m_document.pageCount = m_api->FPDF_GetPageCount(document);

		m_initialized = true;
	}

	PDFDocumentData::PDFDocumentData(std::unique_ptr<IReader> reader, const StringView password, const PDFiumAPI* api)
	{
		m_api = api;

		FPDF_DOCUMENT document = nullptr;
		{
			Blob blob{ std::move(reader) };
			const void* data = blob.data();
			const int32 size_bytes = static_cast<int32>(blob.size_bytes());

			if (password)
			{
				document = m_api->FPDF_LoadMemDocument(data, size_bytes, Unicode::ToUTF8(password).c_str());
			}
			else
			{
				document = m_api->FPDF_LoadMemDocument(data, size_bytes, nullptr);
			}

			if (not document)
			{
				LogPDFiumError(m_api, static_cast<bool>(password));
				return;
			}

			m_document.blob = std::move(blob);
		}

		m_document.data = document;
		m_document.version = GetVersion(m_api, document);
		m_document.pageCount = m_api->FPDF_GetPageCount(document);

		m_initialized = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	isInitialized
	//
	////////////////////////////////////////////////////////////////

	bool PDFDocumentData::isInitialized() const noexcept
	{
		return m_initialized;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getInfo
	//
	////////////////////////////////////////////////////////////////

	String PDFDocumentData::getInfo() const
	{
		return m_document.getInfo();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isOpen
	//
	////////////////////////////////////////////////////////////////

	bool PDFDocumentData::isOpen() const noexcept
	{
		return static_cast<bool>(m_document);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getVersion
	//
	////////////////////////////////////////////////////////////////

	const String& PDFDocumentData::getVersion() const
	{
		return m_document.version;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPageCount
	//
	////////////////////////////////////////////////////////////////

	int32 PDFDocumentData::getPageCount() const
	{
		return m_document.pageCount;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPageSize
	//
	////////////////////////////////////////////////////////////////

	Size PDFDocumentData::getPageSize(const int32 pageIndex, const double scale) const
	{
		if (not isOpen())
		{
			return{ 0,0 };
		}

		if ((pageIndex < 0) || (m_document.pageCount <= pageIndex))
		{
			return{ 0,0 };
		}

		FS_SIZEF pageSize{ 0.0f, 0.0f };
		m_api->FPDF_GetPageSizeByIndexF(m_document.data, pageIndex, &pageSize);

		const double pageWidthPt = pageSize.width;
		const double pageHeightPt = pageSize.height;

		const int32 width = Clamp(static_cast<int32>(Math::Round(pageWidthPt * scale)), 1, Image::MaxWidth);
		const int32 height = Clamp(static_cast<int32>(Math::Round(pageHeightPt * scale)), 1, Image::MaxHeight);

		return{ width, height };
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderPage
	//
	////////////////////////////////////////////////////////////////

	bool PDFDocumentData::renderPage(const int32 pageIndex, PDFPage& result, const PDFRenderOptions& options) const
	{
		const double scale = Max(0.0, options.scale);

		result.image.clear();
		result.scale = scale;
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

		// 画像
		if (not RenderImage(m_api, page, options, result))
		{
			return false;
		}

		// リンク注釈
		result.links = CollectLinks(m_api, m_document.data, page, m_document.pageCount, m_api->FPDF_GetPageHeightF(page), scale);

		return true;
	}

	PDFDocumentData::Document::operator bool() const noexcept
	{
		return (data != nullptr);
	}

	String PDFDocumentData::Document::getInfo() const
	{
		return U"(version = {}, pageCount = {})"_fmt(version, pageCount);
	}
}

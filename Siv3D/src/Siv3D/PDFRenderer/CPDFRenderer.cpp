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

# include "CPDFRenderer.hpp"
# include <Siv3D/PDFPage.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>

namespace s3d
{
	CPDFRenderer::~CPDFRenderer()
	{
		LOG_SCOPED_DEBUG("CPDFRenderer::~CPDFRenderer()");

		m_documents.destroy();

		if (m_state == State::Ready)
		{
			m_api.FPDF_DestroyLibrary();
			m_state = State::Uninitialized;
		}

		if (m_library)
		{
			DLL::Unload(m_library);
			m_library = nullptr;
		}
	}

	void CPDFRenderer::init()
	{
		assert(m_state == State::Uninitialized);
		LOG_SCOPED_DEBUG("CPDFRenderer::init()");

		// null PDFDocument を管理に登録
		{
			auto nullDocument = std::make_unique<PDFDocumentData>(PDFDocumentData::Null{});

			if (not nullDocument->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null PDFDocument" };
			}

			m_documents.setNullData(std::move(nullDocument));
		}
	}

	PDFDocument::IDType CPDFRenderer::createFromFile(const FilePathView path, const StringView password)
	{
		if (not path)
		{
			return PDFDocument::IDType::Null();
		}

		if (m_state == State::Uninitialized)
		{
			setup();
		}
		
		if (m_state != State::Ready)
		{
			return PDFDocument::IDType::Null();
		}

		std::unique_ptr<PDFDocumentData> document = std::make_unique<PDFDocumentData>(path, password, & m_api);

		if (not document->isInitialized())
		{
			return PDFDocument::IDType::Null();
		}

		const String info = document->getInfo();
		return m_documents.add(std::move(document), info);
	}

	void CPDFRenderer::release(const PDFDocument::IDType handleID)
	{
		if (m_state == State::Uninitialized)
		{
			setup();
		}

		if (m_state != State::Ready)
		{
			return;
		}

		m_documents.erase(handleID);
	}

	const String& CPDFRenderer::getVersion(const PDFDocument::IDType handleID)
	{
		if (m_state == State::Uninitialized)
		{
			setup();
		}
		
		if (m_state != State::Ready)
		{
			static const String nullVersion{};
			return nullVersion;
		}

		return m_documents[handleID]->getVersion();
	}

	int32 CPDFRenderer::getPageCount(const PDFDocument::IDType handleID)
	{
		if (m_state == State::Uninitialized)
		{
			setup();
		}
		
		if (m_state != State::Ready)
		{
			return 0;
		}
		
		return m_documents[handleID]->getPageCount();
	}

	Size CPDFRenderer::getPageSize(const PDFDocument::IDType handleID, const int32 pageIndex, const double scale)
	{
		if (m_state == State::Uninitialized)
		{
			setup();
		}

		if (m_state != State::Ready)
		{
			return{ 0, 0 };
		}

		return m_documents[handleID]->getPageSize(pageIndex, scale);
	}

	bool CPDFRenderer::renderPage(const PDFDocument::IDType handleID, const int32 pageIndex, PDFPage& result, const PDFRenderOptions& options)
	{
		if (m_state == State::Uninitialized)
		{
			setup();
		}

		if (m_state != State::Ready)
		{
			result.clear();
			return false;
		}

		return m_documents[handleID]->renderPage(pageIndex, result, options);
	}

	void CPDFRenderer::setup()
	{
		assert(m_state == State::Uninitialized);
		assert(not m_library);
		
		m_library = DLL::Load(U"engine/dll/pdfium.dll");
		if (not m_library)
		{
			LOG_FAIL(U"Failed to load `engine/dll/pdfium.dll`");
			m_state = State::Failed;
			return;
		}

		if (not m_api.init(m_library))
		{
			LOG_FAIL(U"Failed to initialize PDFiumAPI");
			DLL::Unload(m_library);
			m_library = nullptr;
			m_state = State::Failed;
			return;
		}

		m_api.FPDF_InitLibrary();
		m_state = State::Ready;
	}
}

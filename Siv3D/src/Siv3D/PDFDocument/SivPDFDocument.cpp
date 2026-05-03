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

# include <cstdlib>
# include <Siv3D/PDFDocument.hpp>
# include <Siv3D/PDFRenderer/IPDFRenderer.hpp>
# include <Siv3D/AssetMonitor/IAssetMonitor.hpp>
# include <Siv3D/Troubleshooting/Troubleshooting.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		static void CheckEngine()
		{
			if (not Siv3DEngine::isAvailable())
			{
				Troubleshooting::Show(Troubleshooting::Error::AssetInitializationBeforeEngineStartup, U"PDFDocument");
				std::exit(EXIT_FAILURE);
			}
		}
	}

	template <>
	AssetIDWrapper<AssetHandle<PDFDocument>>::AssetIDWrapper()
	{
		CheckEngine();
	}

	template <>
	AssetIDWrapper<AssetHandle<PDFDocument>>::~AssetIDWrapper()
	{
		if (not Siv3DEngine::isAvailable())
		{
			return;
		}

		SIV3D_ENGINE(PDFRenderer)->release(m_id);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	PDFDocument::PDFDocument() {}

	PDFDocument::PDFDocument(const FilePathView path, const StringView password)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(PDFRenderer)->createFromFile(path, password))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

	PDFDocument::PDFDocument(std::unique_ptr<IReader> reader, const StringView password)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(PDFRenderer)->createFromReader(std::move(reader), password)))}
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	PDFDocument::~PDFDocument() {}

	////////////////////////////////////////////////////////////////
	//
	//	version
	//
	////////////////////////////////////////////////////////////////

	const String& PDFDocument::version() const
	{
		return SIV3D_ENGINE(PDFRenderer)->getVersion(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	pageCount
	//
	////////////////////////////////////////////////////////////////

	int32 PDFDocument::pageCount() const
	{
		return SIV3D_ENGINE(PDFRenderer)->getPageCount(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPageSize
	//
	////////////////////////////////////////////////////////////////

	Size PDFDocument::getPageSize(const int32 pageIndex, const double scale) const
	{
		return SIV3D_ENGINE(PDFRenderer)->getPageSize(m_handle->id(), pageIndex, scale);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderPage
	//
	////////////////////////////////////////////////////////////////

	PDFPage PDFDocument::renderPage(const int32 pageIndex, const PDFRenderOptions& options) const
	{
		PDFPage result;
		
		if (not SIV3D_ENGINE(PDFRenderer)->renderPage(m_handle->id(), pageIndex, result, options))
		{
			return{};
		}
		
		return result;
	}

	bool PDFDocument::renderPage(const int32 pageIndex, PDFPage& result, const PDFRenderOptions& options) const
	{
		return SIV3D_ENGINE(PDFRenderer)->renderPage(m_handle->id(), pageIndex, result, options);
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void PDFDocument::swap(PDFDocument& other) noexcept
	{
		m_handle.swap(other.m_handle);
	}
}

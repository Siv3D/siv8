//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CScreenCapture.hpp"
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CScreenCapture::~CScreenCapture()
	{
		LOG_SCOPED_DEBUG("CScreenCapture::~CScreenCapture()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CScreenCapture::init()
	{
		LOG_SCOPED_DEBUG("CScreenCapture::init()");

	# if SIV3D_PLATFORM(MACOS)

		m_screenshotSaveDirectory = (FileSystem::GetFolderPath(SpecialFolder::Pictures) + U"Screenshot/");

	# else

		m_screenshotSaveDirectory = (FileSystem::InitialDirectory() + U"Screenshot/");

	# endif

		LOG_INFO(fmt::format("Default Screenshot directory: \"{}\"", m_screenshotSaveDirectory));
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CScreenCapture::update()
	{

	}

	////////////////////////////////////////////////////////////////
	//
	//	getScreenshotSaveDirectory
	//
	////////////////////////////////////////////////////////////////

	const FilePath& CScreenCapture::getScreenshotSaveDirectory() const noexcept
	{
		return m_screenshotSaveDirectory;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setScreenshotSaveDirectory
	//
	////////////////////////////////////////////////////////////////

	void CScreenCapture::setScreenshotSaveDirectory(const FilePathView path)
	{
		m_screenshotSaveDirectory = path;
	}

	////////////////////////////////////////////////////////////////
	//
	//	requestScreenCapture
	//
	////////////////////////////////////////////////////////////////

	void CScreenCapture::requestScreenCapture()
	{
		m_requestedPaths.emplace_back();
	}

	void CScreenCapture::requestScreenCapture(const FilePathView path)
	{
		m_requestedPaths.emplace_back(path);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNewFrame
	//
	////////////////////////////////////////////////////////////////

	bool CScreenCapture::hasNewFrame() const noexcept
	{
		return m_hasNewFrame;
	}

	////////////////////////////////////////////////////////////////
	//
	//	receiveScreenCapture
	//
	////////////////////////////////////////////////////////////////

	const Image& CScreenCapture::receiveScreenCapture() const
	{
		return SIV3D_ENGINE(Renderer)->getScreenCapture();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setScreenshotShortcutKeys
	//
	////////////////////////////////////////////////////////////////

	void CScreenCapture::setScreenshotShortcutKeys(const Array<InputGroup>& screenshotShortcutKeys)
	{
		m_screenshotShortcutKeys = screenshotShortcutKeys;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getScreenshotShortcutKeys
	//
	////////////////////////////////////////////////////////////////

	const Array<InputGroup>& CScreenCapture::getScreenshotShortcutKeys() const noexcept
	{
		return m_screenshotShortcutKeys;
	}
}

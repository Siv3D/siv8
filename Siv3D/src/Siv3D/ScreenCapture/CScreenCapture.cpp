//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CScreenCapture.hpp"
# include <Siv3D/ScreenCapture.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/SpecialFolder.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/Renderer/IRenderer.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool CaptureKeyDown(const Array<InputGroup>& screenshotShortcutKeys)
		{
			for (const auto& inputGroup : screenshotShortcutKeys)
			{
				if (inputGroup.down())
				{
					return true;
				}
			}

			return false;
		}

		static void SaveScreenCapture(const Image& image, const FilePath& m_screenshotSaveDirectory, const Array<FilePath>& paths)
		{
			for (const auto& path : paths)
			{
				// 空のパスは「画像ファイルに保存しない」スクリーンキャプチャのリクエスト
				if (path.isEmpty())
				{
					continue;
				}
			
				const FilePath filePath = (m_screenshotSaveDirectory + path);
				
				image.save(filePath);

				LOG_INFO(fmt::format("📷 Screen capture saved (path: \"{0}\")", filePath));
			}
		}
	}

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
		m_hasNewFrame = false;

		// スクリーンショットのキー入力をチェック
		if ((not m_requestedPaths) && CaptureKeyDown(m_screenshotShortcutKeys))
		{
			ScreenCapture::SaveCurrentFrame();
		}

		if (not m_requestedPaths)
		{
			return;
		}

		SIV3D_ENGINE(Renderer)->captureScreenshot();

		const Image& image = SIV3D_ENGINE(Renderer)->getScreenCapture();

		if (not image)
		{
			LOG_FAIL("✖ failed to capture a screen shot");
		}

		// スクリーンショットの保存
		SaveScreenCapture(image, m_screenshotSaveDirectory, m_requestedPaths);

		m_requestedPaths.clear();

		m_hasNewFrame = true;
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

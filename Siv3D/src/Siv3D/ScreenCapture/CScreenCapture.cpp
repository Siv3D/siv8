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

		static void SaveScreenCapture(const Image& image, const Array<FilePath>& paths)
		{
			for (const auto& path : paths)
			{
				// 空のパスは「画像ファイルに保存しない」スクリーンキャプチャのリクエスト
				if (path.isEmpty())
				{
					continue;
				}
			
				if (image.save(path))
				{
					LOG_INFO(fmt::format("📷 Screen capture saved (path: \"{}\")", path));
				}
				else
				{
					LOG_FAIL(fmt::format("Screen capture save failed (path: \"{}\")", path));
				}
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

		const auto& pictures = FileSystem::GetFolderPath(SpecialFolder::Pictures);
		if (pictures)
		{
			m_screenshotSaveDirectory = FileSystem::PathAppend(pictures, U"Screenshot/");
		}
		else
		{
			LOG_FAIL("ScreenCapture: Pictures directory is unavailable");
		}

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
			m_requestedPaths.clear();
			return;
		}

		// スクリーンショットの保存
		SaveScreenCapture(image, m_requestedPaths);

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
		if (path.isEmpty() || path.contains(U'\0') || FileSystem::IsResourcePath(path))
		{
			LOG_FAIL("ScreenCapture::SetScreenshotDirectory(): invalid directory path");
			return;
		}

		FilePath directory = FileSystem::FullPath(path);
		if (directory.isEmpty())
		{
			LOG_FAIL("ScreenCapture::SetScreenshotDirectory(): failed to resolve directory path");
			return;
		}

		if (not directory.ends_with(U'/'))
		{
			directory.push_back(U'/');
		}

		m_screenshotSaveDirectory = std::move(directory);
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
		if (path.isEmpty() || path.contains(U'\0'))
		{
			LOG_FAIL("ScreenCapture::SaveCurrentFrameTo(): invalid file path");
			return;
		}

		FilePath fullPath = FileSystem::FullPath(path);
		if (fullPath.isEmpty())
		{
			LOG_FAIL("ScreenCapture::SaveCurrentFrameTo(): failed to resolve file path");
			return;
		}

		m_requestedPaths.push_back(std::move(fullPath));
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

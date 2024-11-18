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

# include <Siv3D/ScreenCapture.hpp>
# include <Siv3D/Image.hpp>
//# include <Siv3D/DynamicTexture.hpp>
# include <Siv3D/ScreenCapture/IScreenCapture.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace ScreenCapture
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetScreenshotDirectory
		//
		////////////////////////////////////////////////////////////////

		FilePath GetScreenshotDirectory()
		{
			return SIV3D_ENGINE(ScreenCapture)->getScreenshotSaveDirectory();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetScreenshotDirectory
		//
		////////////////////////////////////////////////////////////////

		void SetScreenshotDirectory(const FilePathView path)
		{
			SIV3D_ENGINE(ScreenCapture)->setScreenshotSaveDirectory(path);
		}

		////////////////////////////////////////////////////////////////
		//
		//	SaveCurrentFrame
		//
		////////////////////////////////////////////////////////////////

		void SaveCurrentFrame(const FilePathView path)
		{
			SIV3D_ENGINE(ScreenCapture)->requestScreenCapture(path);
		}

		////////////////////////////////////////////////////////////////
		//
		//	RequestCurrentFrame
		//
		////////////////////////////////////////////////////////////////

		void RequestCurrentFrame()
		{
			SIV3D_ENGINE(ScreenCapture)->requestScreenCapture({});
		}

		////////////////////////////////////////////////////////////////
		//
		//	HasNewFrame
		//
		////////////////////////////////////////////////////////////////

		bool HasNewFrame()
		{
			return SIV3D_ENGINE(ScreenCapture)->hasNewFrame();
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetFrame
		//
		////////////////////////////////////////////////////////////////

		const Image& GetFrame()
		{
			return SIV3D_ENGINE(ScreenCapture)->receiveScreenCapture();
		}

		bool GetFrame(Image& image)
		{
			image = SIV3D_ENGINE(ScreenCapture)->receiveScreenCapture();

			return static_cast<bool>(image);
		}

		//bool GetFrame(DynamicTexture& texture)
		//{
		//	return texture.fill(SIV3D_ENGINE(ScreenCapture)->receiveScreenCapture());
		//}

		////////////////////////////////////////////////////////////////
		//
		//	SetShortcutKeys
		//
		////////////////////////////////////////////////////////////////

		void SetShortcutKeys(const Array<InputGroup>& screenshotShortcutKeys)
		{
			SIV3D_ENGINE(ScreenCapture)->setScreenshotShortcutKeys(screenshotShortcutKeys);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetShortcutKeys
		//
		////////////////////////////////////////////////////////////////

		const Array<InputGroup>& GetShortcutKeys() noexcept
		{
			return SIV3D_ENGINE(ScreenCapture)->getScreenshotShortcutKeys();
		}
	}
}

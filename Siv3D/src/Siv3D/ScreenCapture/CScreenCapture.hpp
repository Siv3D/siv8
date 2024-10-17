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

# pragma once
# include <Siv3D/Keyboard.hpp>
# include "IScreenCapture.hpp"

namespace s3d
{
	class CScreenCapture final : public ISiv3DScreenCapture
	{
	public:

		~CScreenCapture() override;

		void init() override;

		void update() override;

		const FilePath& getScreenshotSaveDirectory() const noexcept override;

		void setScreenshotSaveDirectory(FilePathView path) override;

		void requestScreenCapture() override;

		void requestScreenCapture(FilePathView path) override;

		bool hasNewFrame() const noexcept override;

		const Image& receiveScreenCapture() const override;

		void setScreenshotShortcutKeys(const Array<InputGroup>& screenshotShortcutKeys) override;

		const Array<InputGroup>& getScreenshotShortcutKeys() const noexcept override;

	private:

		FilePath m_screenshotSaveDirectory;

		Array<FilePath> m_requestedPaths;

		Array<InputGroup> m_screenshotShortcutKeys = { KeyPrintScreen, KeyF12 };

		bool m_hasNewFrame = false;
	};
}

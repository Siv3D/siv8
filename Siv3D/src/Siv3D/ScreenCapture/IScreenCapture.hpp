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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/InputGroups.hpp>

namespace s3d
{
	class Image;

	class SIV3D_NOVTABLE ISiv3DScreenCapture
	{
	public:

		[[nodiscard]]
		static ISiv3DScreenCapture* Create();

		virtual ~ISiv3DScreenCapture() = default;

		virtual void init() = 0;

		virtual void update() = 0;

		virtual const FilePath& getScreenshotSaveDirectory() const noexcept = 0;

		virtual void setScreenshotSaveDirectory(FilePathView path) = 0;

		virtual void requestScreenCapture() = 0;

		virtual void requestScreenCapture(FilePathView path) = 0;

		virtual bool hasNewFrame() const noexcept = 0;

		virtual const Image& receiveScreenCapture() const = 0;

		virtual void setScreenshotShortcutKeys(const Array<InputGroup>& screenshotShortcutKeys) = 0;

		virtual const Array<InputGroup>& getScreenshotShortcutKeys() const noexcept = 0;
	};
}

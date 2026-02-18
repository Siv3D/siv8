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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/PredefinedYesNo.hpp>

namespace s3d
{
	class Image;
	class Blob;

	class SIV3D_NOVTABLE ISiv3DClipboard
	{
	public:

		[[nodiscard]]
		static ISiv3DClipboard* Create();

		virtual ~ISiv3DClipboard() = default;

		virtual void init() = 0;

		virtual bool hasChanged() = 0;

		virtual uint64 getSequenceNumber() = 0;

		virtual void clear() = 0;

		virtual void setText(StringView text) = 0;

		virtual bool getText(String& text) = 0;

		virtual bool hasText() = 0;

		virtual void setImage(const Image& image) = 0;

		virtual bool getImage(Image& image, PremultiplyAlpha premultiplyAlpha) = 0;

		virtual bool hasImage() = 0;

		virtual void setFilePaths(const Array<FilePath>& paths) = 0;

		virtual bool getFilePaths(Array<FilePath>& paths) = 0;

		virtual void setRichText(StringView richText, StringView plainTextFallback) = 0;

		virtual void setHTML(StringView html, StringView plainTextFallback) = 0;

		virtual void setData(StringView mimeType, const void* data, size_t size) = 0;

		virtual bool getData(StringView mimeType, Blob& data) = 0;
		
		virtual Array<String> getAvailableMimeTypes() = 0;
	};
}

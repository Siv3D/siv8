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
# include "Common.hpp"
# include "String.hpp"
# include "Array.hpp"
# include "Image.hpp"
# include "Optional.hpp"
# include "Blob.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	namespace Clipboard
	{
		////////////////////////////////////////////////////////////////
		//
		//	HasChanged
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool HasChanged();

		////////////////////////////////////////////////////////////////
		//
		//	GetSequenceNumber
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 GetSequenceNumber();

		////////////////////////////////////////////////////////////////
		//
		//	Clear
		//
		////////////////////////////////////////////////////////////////

		void Clear();

		////////////////////////////////////////////////////////////////
		//
		//	SetText, GetText, HasText
		//
		////////////////////////////////////////////////////////////////

		void SetText(StringView text);

		[[nodiscard]]
		Optional<String> GetText();

		bool GetText(String& text);

		[[nodiscard]]
		bool HasText();

		////////////////////////////////////////////////////////////////
		//
		//	SetImage, GetImage, HasImage
		//
		////////////////////////////////////////////////////////////////

		/// @brief 画像をクリップボードにセットします。 | Sets the image to the clipboard.
		/// @param image 画像（非アルファ乗算） | Image (non-premultiplied alpha)
		void SetImage(const Image& image);

		[[nodiscard]]
		Optional<Image> GetImage(PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes);

		bool GetImage(Image& image, PremultiplyAlpha premultiplyAlpha = PremultiplyAlpha::Yes);

		[[nodiscard]]
		bool HasImage();

		////////////////////////////////////////////////////////////////
		//
		//	SetFilePaths, GetFilePaths
		//
		////////////////////////////////////////////////////////////////

		void SetFilePaths(const Array<FilePath>& paths);

		[[nodiscard]]
		Optional<Array<FilePath>> GetFilePaths();

		[[nodiscard]]
		bool GetFilePaths(Array<FilePath>& paths);

		////////////////////////////////////////////////////////////////
		//
		//	SetRichText
		//
		////////////////////////////////////////////////////////////////

		void SetRichText(StringView richText, StringView plainTextFallback = U"");

		////////////////////////////////////////////////////////////////
		//
		//	SetHTML
		//
		////////////////////////////////////////////////////////////////

		void SetHTML(StringView html, StringView plainTextFallback = U"");

		////////////////////////////////////////////////////////////////
		//
		//	SetData, GetData
		//
		////////////////////////////////////////////////////////////////

		void SetData(StringView mimeType, const void* data, size_t size);

		void SetData(StringView mimeType, std::span<const Byte> data);

		[[nodiscard]]
		Optional<Blob> GetData(StringView mimeType);

		bool GetData(StringView mimeType, Blob& data);

		////////////////////////////////////////////////////////////////
		//
		//	GetAvailableMimeTypes
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<String> GetAvailableMimeTypes();
	}
}

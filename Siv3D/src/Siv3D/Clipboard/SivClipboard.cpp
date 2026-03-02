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

# include <Siv3D/Clipboard.hpp>
# include <Siv3D/Clipboard/IClipboard.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace Clipboard
	{
		////////////////////////////////////////////////////////////////
		//
		//	HasChanged
		//
		////////////////////////////////////////////////////////////////

		bool HasChanged()
		{
			return SIV3D_ENGINE(Clipboard)->hasChanged();
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetSequenceNumber
		//
		////////////////////////////////////////////////////////////////

		uint64 GetSequenceNumber()
		{
			return SIV3D_ENGINE(Clipboard)->getSequenceNumber();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Clear
		//
		////////////////////////////////////////////////////////////////

		void Clear()
		{
			SIV3D_ENGINE(Clipboard)->clear();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetText, GetText, HasText
		//
		////////////////////////////////////////////////////////////////

		void SetText(const StringView text)
		{
			SIV3D_ENGINE(Clipboard)->setText(text);
		}

		Optional<String> GetText()
		{
			if (String text; SIV3D_ENGINE(Clipboard)->getText(text))
			{
				return text;
			}
			else
			{
				return none;
			}
		}

		bool GetText(String& text)
		{
			return SIV3D_ENGINE(Clipboard)->getText(text);
		}

		bool HasText()
		{
			return SIV3D_ENGINE(Clipboard)->hasText();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetImage, GetImage, HasImage
		//
		////////////////////////////////////////////////////////////////

		void SetImage(const Image& image)
		{
			SIV3D_ENGINE(Clipboard)->setImage(image);
		}

		Optional<Image> GetImage(const PremultiplyAlpha premultiplyAlpha)
		{
			if (Image image; SIV3D_ENGINE(Clipboard)->getImage(image, premultiplyAlpha))
			{
				return image;
			}
			else
			{
				return none;
			}
		}

		bool GetImage(Image& image, const PremultiplyAlpha premultiplyAlpha)
		{
			return SIV3D_ENGINE(Clipboard)->getImage(image, premultiplyAlpha);
		}

		bool HasImage()
		{
			return SIV3D_ENGINE(Clipboard)->hasImage();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetFilePaths, GetFilePaths
		//
		////////////////////////////////////////////////////////////////

		void SetFilePaths(const Array<FilePath>& paths)
		{
			SIV3D_ENGINE(Clipboard)->setFilePaths(paths);
		}

		Optional<Array<FilePath>> GetFilePaths()
		{
			if (Array<FilePath> paths; SIV3D_ENGINE(Clipboard)->getFilePaths(paths))
			{
				return paths;
			}
			else
			{
				return none;
			}
		}

		bool GetFilePaths(Array<FilePath>& paths)
		{
			return SIV3D_ENGINE(Clipboard)->getFilePaths(paths);
		}

		bool HasFilePaths()
		{
			return SIV3D_ENGINE(Clipboard)->hasFilePaths();
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetRichText
		//
		////////////////////////////////////////////////////////////////

		void SetRichText(const StringView richText)
		{
			SIV3D_ENGINE(Clipboard)->setRichText(richText, none);
		}

		void SetRichText(const StringView richText, const StringView plainTextFallback)
		{
			SIV3D_ENGINE(Clipboard)->setRichText(richText, plainTextFallback);
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetHTML
		//
		////////////////////////////////////////////////////////////////

		void SetHTML(const StringView html)
		{
			SIV3D_ENGINE(Clipboard)->setHTML(html, none);
		}

		void SetHTML(const StringView html, const StringView plainTextFallback)
		{
			SIV3D_ENGINE(Clipboard)->setHTML(html, plainTextFallback);
		}

		////////////////////////////////////////////////////////////////
		//
		//	SetData, GetData
		//
		////////////////////////////////////////////////////////////////

		void SetData(const StringView mimeType, const void* data, const size_t size)
		{
			SIV3D_ENGINE(Clipboard)->setData(mimeType, data, size, none);
		}

		void SetData(const StringView mimeType, const std::span<const Byte> data)
		{
			SIV3D_ENGINE(Clipboard)->setData(mimeType, data.data(), data.size_bytes(), none);
		}

		void SetData(const StringView mimeType, const void* data, const size_t size, const StringView plainTextFallback)
		{
			SIV3D_ENGINE(Clipboard)->setData(mimeType, data, size, plainTextFallback);
		}

		void SetData(const StringView mimeType, const std::span<const Byte> data, const StringView plainTextFallback)
		{
			SIV3D_ENGINE(Clipboard)->setData(mimeType, data.data(), data.size_bytes(), plainTextFallback);
		}

		Optional<Blob> GetData(const StringView mimeType)
		{
			if (Blob data; SIV3D_ENGINE(Clipboard)->getData(mimeType, data))
			{
				return data;
			}
			else
			{
				return none;
			}
		}

		bool GetData(const StringView mimeType, Blob& data)
		{
			return SIV3D_ENGINE(Clipboard)->getData(mimeType, data);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetAvailableMimeTypes
		//
		////////////////////////////////////////////////////////////////

		Array<String> GetAvailableMimeTypes()
		{
			return SIV3D_ENGINE(Clipboard)->getAvailableMimeTypes();
		}
	}
}

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

namespace s3d
{
	namespace ImageDecoder
	{
		////////////////////////////////////////////////////////////////
		//
		//	Decode
		//
		////////////////////////////////////////////////////////////////

		template <ReaderObject Reader>
		Image Decode(Reader&& reader, const PremultiplyAlpha premultiplyAlpha, const ImageFormat imageFormat)
		{
			return Decode(std::make_unique<Reader>(std::forward<Reader>(reader)), premultiplyAlpha, imageFormat);
		}

		template <ReaderObject Reader>
		Image Decode(Reader&& reader, const PremultiplyAlpha premultiplyAlpha, const StringView decoderName)
		{
			return Decode(std::make_unique<Reader>(std::forward<Reader>(reader)), premultiplyAlpha, decoderName);
		}

		////////////////////////////////////////////////////////////////
		//
		//	DecodeGray16
		//
		////////////////////////////////////////////////////////////////

		template <ReaderObject Reader>
		Grid<uint16> DecodeGray16(Reader&& reader, const ImageFormat imageFormat)
		{
			return DecodeGray16(std::make_unique<Reader>(std::forward<Reader>(reader)), imageFormat);
		}

		template <ReaderObject Reader>
		Grid<uint16> DecodeGray16(Reader&& reader, StringView decoderName)
		{
			return DecodeGray16(std::make_unique<Reader>(std::forward<Reader>(reader)), decoderName);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		template <class ImageDecoder>
			requires std::derived_from<ImageDecoder, IImageDecoder>
		bool Add()
		{
			return Add(std::make_unique<ImageDecoder>());
		}
	}
}

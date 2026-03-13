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

# include <Siv3D/ImageDecoder.hpp>
# include <Siv3D/BinaryFileReader.hpp>
# include "IImageDecoder.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace ImageDecoder
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetImageInfo
		//
		////////////////////////////////////////////////////////////////

		Optional<ImageInfo> GetImageInfo(const FilePathView path, const ImageFormat imageFormat)
		{
			return GetImageInfo(path, Format(imageFormat));
		}

		Optional<ImageInfo> GetImageInfo(const FilePathView path, const StringView decoderName)
		{
			BinaryFileReader reader{ path };

			if (not reader)
			{
				return{};
			}

			return SIV3D_ENGINE(ImageDecoder)->getImageInfo(reader, path, decoderName);
		}

		Optional<ImageInfo> GetImageInfo(const IReader& reader, const ImageFormat imageFormat)
		{
			return GetImageInfo(reader, Format(imageFormat));
		}

		Optional<ImageInfo> GetImageInfo(const IReader& reader, const StringView decoderName)
		{
			return SIV3D_ENGINE(ImageDecoder)->getImageInfo(reader, {}, decoderName);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Decode
		//
		////////////////////////////////////////////////////////////////

		Image Decode(const FilePathView path, const PremultiplyAlpha premultiplyAlpha, const ImageFormat imageFormat)
		{
			return Decode(path, premultiplyAlpha, Format(imageFormat));
		}

		Image Decode(const FilePathView path, const PremultiplyAlpha premultiplyAlpha, const StringView decoderName)
		{
			std::unique_ptr<BinaryFileReader> reader = std::make_unique<BinaryFileReader>(path);

			if (not reader->isOpen())
			{
				return{};
			}

			return SIV3D_ENGINE(ImageDecoder)->decode(std::move(reader), path, premultiplyAlpha, decoderName);
		}

		Image Decode(std::unique_ptr<IReader> reader, const PremultiplyAlpha premultiplyAlpha, const ImageFormat imageFormat)
		{
			return Decode(std::move(reader), premultiplyAlpha, Format(imageFormat));
		}

		Image Decode(std::unique_ptr<IReader> reader, const PremultiplyAlpha premultiplyAlpha, const StringView decoderName)
		{
			return SIV3D_ENGINE(ImageDecoder)->decode(std::move(reader), {}, premultiplyAlpha, decoderName);
		}

		////////////////////////////////////////////////////////////////
		//
		//	DecodeGray16
		//
		////////////////////////////////////////////////////////////////

		Grid<uint16> DecodeGray16(const FilePathView path, const ImageFormat imageFormat)
		{
			return DecodeGray16(path, Format(imageFormat));
		}

		Grid<uint16> DecodeGray16(const FilePathView path, const StringView decoderName)
		{
			std::unique_ptr<BinaryFileReader> reader = std::make_unique<BinaryFileReader>(path);

			if (not reader->isOpen())
			{
				return{};
			}

			return SIV3D_ENGINE(ImageDecoder)->decodeGray16(std::move(reader), path, decoderName);
		}

		Grid<uint16> DecodeGray16(std::unique_ptr<IReader> reader, const ImageFormat imageFormat)
		{
			return DecodeGray16(std::move(reader), Format(imageFormat));
		}

		Grid<uint16> DecodeGray16(std::unique_ptr<IReader> reader, const StringView decoderName)
		{
			return SIV3D_ENGINE(ImageDecoder)->decodeGray16(std::move(reader), {}, decoderName);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		bool Add(std::unique_ptr<IImageDecoder> decoder)
		{
			return SIV3D_ENGINE(ImageDecoder)->add(std::move(decoder));
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Remove
		//
		////////////////////////////////////////////////////////////////

		void Remove(const StringView decoderName)
		{
			return SIV3D_ENGINE(ImageDecoder)->remove(decoderName);
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	IsRegistered
		//
		////////////////////////////////////////////////////////////////

		bool IsRegistered(const StringView decoderName) noexcept
		{
			for (const auto& decoder : SIV3D_ENGINE(ImageDecoder)->enumDecoder())
			{
				if (decoder->name() == decoderName)
				{
					return true;
				}
			}

			return false;
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Enum
		//
		////////////////////////////////////////////////////////////////

		const Array<std::unique_ptr<IImageDecoder>>& Enum() noexcept
		{
			return SIV3D_ENGINE(ImageDecoder)->enumDecoder();
		}
	}
}

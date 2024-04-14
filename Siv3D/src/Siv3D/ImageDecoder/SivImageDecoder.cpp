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

# include <Siv3D/ImageDecoder.hpp>
# include <Siv3D/BinaryReader.hpp>
# include "IImageDecoder.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace ImageDecoder
	{
		Optional<ImageInfo> GetImageInfo(const FilePathView path, const ImageFormat imageFormat)
		{
			BinaryReader reader{ path };

			if (not reader)
			{
				return{};
			}

			return SIV3D_ENGINE(ImageDecoder)->getImageInfo(reader, path, imageFormat);
		}

		Optional<ImageInfo> GetImageInfo(IReader& reader, const ImageFormat imageFormat)
		{
			return SIV3D_ENGINE(ImageDecoder)->getImageInfo(reader, {}, imageFormat);
		}

		Image Decode(const FilePathView path, const PremultiplyAlpha premultiplyAlpha, const ImageFormat imageFormat)
		{
			BinaryReader reader{ path };

			if (not reader)
			{
				return{};
			}

			return SIV3D_ENGINE(ImageDecoder)->decode(reader, path, premultiplyAlpha, imageFormat);
		}

		Image Decode(IReader& reader, const PremultiplyAlpha premultiplyAlpha, const ImageFormat imageFormat)
		{
			return SIV3D_ENGINE(ImageDecoder)->decode(reader, {}, premultiplyAlpha, imageFormat);
		}

		Grid<uint16> DecodeGray16(const FilePathView path, const ImageFormat imageFormat)
		{
			BinaryReader reader{ path };

			if (not reader)
			{
				return{};
			}

			return SIV3D_ENGINE(ImageDecoder)->decodeGray16(reader, path, imageFormat);
		}

		Grid<uint16> DecodeGray16(IReader& reader, const ImageFormat imageFormat)
		{
			return SIV3D_ENGINE(ImageDecoder)->decodeGray16(reader, {}, imageFormat);
		}

		bool Add(std::unique_ptr<IImageDecoder>&& decoder)
		{
			return SIV3D_ENGINE(ImageDecoder)->add(std::move(decoder));
		}

		void Remove(const StringView decoderName)
		{
			return SIV3D_ENGINE(ImageDecoder)->remove(decoderName);
		}

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

		const Array<std::unique_ptr<IImageDecoder>>& Enum() noexcept
		{
			return SIV3D_ENGINE(ImageDecoder)->enumDecoder();
		}
	}
}

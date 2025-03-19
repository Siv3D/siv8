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

# include <Siv3D/ImageEncoder.hpp>
# include <Siv3D/Image.hpp>
# include "IImageEncoder.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/FileSystem.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr StringView ToString(const ImageFormat imageFormat) noexcept
		{
			switch (imageFormat)
			{
			case ImageFormat::BCn:
				return U"BCn"_sv;
			case ImageFormat::PNG:
				return U"PNG"_sv;
			case ImageFormat::JPEG:
				return U"JPEG"_sv;
			case ImageFormat::JPEG_XL:
				return U"JPEG_XL"_sv;
			case ImageFormat::BMP:
				return U"BMP"_sv;
			case ImageFormat::WebP:
				return U"WebP"_sv;
			case ImageFormat::GIF:
				return U"GIF"_sv;
			case ImageFormat::TIFF:
				return U"TIFF"_sv;
			case ImageFormat::TGA:
				return U"TGA"_sv;
			case ImageFormat::PPM:
				return U"PPM"_sv;
			case ImageFormat::SVG:
				return U"SVG"_sv;
			default:
				return U""_sv;
			}
		}
	}

	namespace ImageEncoder
	{		
		////////////////////////////////////////////////////////////////
		//
		//	Save
		//
		////////////////////////////////////////////////////////////////

		bool Save(const Image& image, ImageFormat format, const FilePathView path)
		{
			if (not image)
			{
				return false;
			}

			String encoderName;

			if (format == ImageFormat::Unspecified)
			{
				encoderName = SIV3D_ENGINE(ImageEncoder)->getEncoderNameFromExtension(FileSystem::Extension(path));
			}
			else
			{
				encoderName = ToString(format);
			}

			return Save(image, encoderName, path);
		}

		bool Save(const Image& image, const StringView encoderName, const FilePathView path)
		{
			return SIV3D_ENGINE(ImageEncoder)->save(image, encoderName, path);
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Encode
		//
		////////////////////////////////////////////////////////////////

		bool Encode(const Image& image, const ImageFormat format, IWriter& writer)
		{
			return Encode(image, ToString(format), writer);
		}

		bool Encode(const Image& image, const StringView encoderName, IWriter& writer)
		{
			return SIV3D_ENGINE(ImageEncoder)->encode(image, encoderName, writer);
		}

		Blob Encode(const Image& image, const ImageFormat format)
		{
			return Encode(image, ToString(format));
		}

		Blob Encode(const Image& image, const StringView encoderName)
		{
			return SIV3D_ENGINE(ImageEncoder)->encode(image, encoderName);
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		bool Add(std::unique_ptr<IImageEncoder>&& Encoder)
		{
			return SIV3D_ENGINE(ImageEncoder)->add(std::move(Encoder));
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Remove
		//
		////////////////////////////////////////////////////////////////

		void Remove(const StringView encoderName)
		{
			return SIV3D_ENGINE(ImageEncoder)->remove(encoderName);
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	IsRegistered
		//
		////////////////////////////////////////////////////////////////

		bool IsRegistered(const StringView encoderName)
		{
			for (const auto& encoder : SIV3D_ENGINE(ImageEncoder)->enumEncoder())
			{
				if (encoder->name() == encoderName)
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

		const Array<std::unique_ptr<IImageEncoder>>& Enum()
		{
			return SIV3D_ENGINE(ImageEncoder)->enumEncoder();
		}
	}
}

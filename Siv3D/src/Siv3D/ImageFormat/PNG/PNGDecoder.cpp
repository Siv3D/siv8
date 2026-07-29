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

# include <Siv3D/ImageFormat/PNGDecoder.hpp>
# include <Siv3D/IReader.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/EngineLog.hpp>
# include <setjmp.h>
# if SIV3D_PLATFORM(WINDOWS) | SIV3D_PLATFORM(MACOS) | SIV3D_PLATFORM(WEB)
#	include <ThirdParty-prebuilt/libpng/png.h>
# else
#	include <png.h>
# endif

namespace s3d
{
	namespace
	{
		struct LookAheadReader
		{
			int64 pos = 0;
			const IReader* reader = nullptr;
		};

		struct PNGReadInfo
		{
			png_uint_32 width = 0;
			png_uint_32 height = 0;
			int bitDepth = 0;
			int colorType = 0;
			int channels = 0;
		};

		static void PngLookAheadCallback(png_structp png_ptr, png_bytep buf, png_size_t length)
		{
			const auto lookAheadReader = static_cast<LookAheadReader*>(::png_get_io_ptr(png_ptr));
			const int64 startPos = lookAheadReader->pos;
			lookAheadReader->reader->lookahead(buf, startPos, length);
			lookAheadReader->pos += length;
		}

		static void PngReadCallback(png_structp png_ptr, png_bytep buf, png_size_t length)
		{
			const auto reader = static_cast<IReader*>(::png_get_io_ptr(png_ptr));

			reader->read(buf, length);
		}

		[[nodiscard]] static bool ReadPNGInfo(png_structp png_ptr, png_infop info_ptr, LookAheadReader* reader, PNGReadInfo* result)
		{
			if (setjmp(png_jmpbuf(png_ptr)))
			{
				return false;
			}

			::png_set_user_limits(png_ptr, static_cast<png_uint_32>(Image::MaxWidth), static_cast<png_uint_32>(Image::MaxHeight));
			::png_set_read_fn(png_ptr, reader, PngLookAheadCallback);
			::png_read_info(png_ptr, info_ptr);

			PNGReadInfo readInfo;
			::png_get_IHDR(png_ptr, info_ptr, &readInfo.width, &readInfo.height, &readInfo.bitDepth, &readInfo.colorType, nullptr, nullptr, nullptr);
			*result = readInfo;

			return true;
		}

		[[nodiscard]] static bool PreparePNGDecode(png_structp png_ptr, png_infop info_ptr, IReader* reader, PNGReadInfo* result)
		{
			if (setjmp(png_jmpbuf(png_ptr)))
			{
				return false;
			}

			::png_set_user_limits(png_ptr, static_cast<png_uint_32>(Image::MaxWidth), static_cast<png_uint_32>(Image::MaxHeight));
			::png_set_read_fn(png_ptr, reader, PngReadCallback);
			::png_read_info(png_ptr, info_ptr);

			PNGReadInfo readInfo;
			::png_get_IHDR(png_ptr, info_ptr, &readInfo.width, &readInfo.height, &readInfo.bitDepth, &readInfo.colorType, nullptr, nullptr, nullptr);

			if ((Image::MaxWidth < readInfo.width) || (Image::MaxHeight < readInfo.height))
			{
				return false;
			}

			if (readInfo.colorType == PNG_COLOR_TYPE_PALETTE)
			{
				::png_set_palette_to_rgb(png_ptr);
			}

			if (::png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			{
				::png_set_tRNS_to_alpha(png_ptr);
			}

			if ((readInfo.colorType == PNG_COLOR_TYPE_GRAY) && (readInfo.bitDepth < 8))
			{
				::png_set_expand_gray_1_2_4_to_8(png_ptr);
			}

			if (readInfo.bitDepth == 16)
			{
				::png_set_scale_16(png_ptr);
			}

			if ((readInfo.colorType == PNG_COLOR_TYPE_GRAY)
				|| (readInfo.colorType == PNG_COLOR_TYPE_GRAY_ALPHA))
			{
				::png_set_gray_to_rgb(png_ptr);
			}

			::png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);

			double gamma;

			if (::png_get_gAMA(png_ptr, info_ptr, &gamma))
			{
				::png_set_gamma(png_ptr, 2.2, gamma);
			}

			::png_read_update_info(png_ptr, info_ptr);
			readInfo.channels = ::png_get_channels(png_ptr, info_ptr);
			*result = readInfo;

			return true;
		}

		[[nodiscard]] static bool PreparePNGGray16Decode(png_structp png_ptr, png_infop info_ptr, IReader* reader, PNGReadInfo* result)
		{
			if (setjmp(png_jmpbuf(png_ptr)))
			{
				return false;
			}

			::png_set_user_limits(png_ptr, static_cast<png_uint_32>(Image::MaxWidth), static_cast<png_uint_32>(Image::MaxHeight));
			::png_set_read_fn(png_ptr, reader, PngReadCallback);
			::png_read_info(png_ptr, info_ptr);

			PNGReadInfo readInfo;
			::png_get_IHDR(png_ptr, info_ptr, &readInfo.width, &readInfo.height, &readInfo.bitDepth, &readInfo.colorType, nullptr, nullptr, nullptr);

			if ((Image::MaxWidth < readInfo.width)
				|| (Image::MaxHeight < readInfo.height)
				|| (readInfo.bitDepth != 16)
				|| (readInfo.colorType != PNG_COLOR_TYPE_GRAY))
			{
				return false;
			}

			// Siv3D supports little-endian platforms only.
			::png_set_swap(png_ptr);
			::png_read_update_info(png_ptr, info_ptr);
			readInfo.channels = ::png_get_channels(png_ptr, info_ptr);
			*result = readInfo;

			return true;
		}

		[[nodiscard]] static bool ReadPNGImage(png_structp png_ptr, png_bytepp rowPointers)
		{
			if (setjmp(png_jmpbuf(png_ptr)))
			{
				return false;
			}

			::png_read_image(png_ptr, rowPointers);
			::png_read_end(png_ptr, nullptr);

			return true;
		}
	}
	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView PNGDecoder::name() const noexcept
	{
		return U"PNG"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	imageFormat
	//
	////////////////////////////////////////////////////////////////

	ImageFormat PNGDecoder::imageFormat() const noexcept
	{
		return ImageFormat::PNG;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isHeader
	//
	////////////////////////////////////////////////////////////////

	bool PNGDecoder::isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept
	{
		static constexpr uint8 signature[] = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };

		return (std::memcmp(bytes, signature, sizeof(signature)) == 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> PNGDecoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"png" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getImageInfo
	//
	////////////////////////////////////////////////////////////////

	Optional<ImageInfo> PNGDecoder::getImageInfo(const FilePathView path) const
	{
		return IImageDecoder::getImageInfo(path);
	}

	Optional<ImageInfo> PNGDecoder::getImageInfo(const IReader& reader, const FilePathView) const
	{
		png_structp png_ptr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (not png_ptr)
		{
			return{};
		}

		ScopeExit cleanup_struct = [&]()
		{
			::png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		};

		png_infop info_ptr = ::png_create_info_struct(png_ptr);

		if (not info_ptr)
		{
			return{};
		}

		ScopeExit cleanup_info = [&]()
		{
			::png_destroy_info_struct(png_ptr, &info_ptr);
		};

		LookAheadReader lookAheadReader{ 0, &reader };
		PNGReadInfo readInfo;

		if (not ReadPNGInfo(png_ptr, info_ptr, &lookAheadReader, &readInfo))
		{
			return{};
		}

		ImagePixelFormat pixelFormat = ImagePixelFormat::R8G8B8A8;

		if (readInfo.colorType == PNG_COLOR_TYPE_GRAY)
		{
			if (readInfo.bitDepth == 16)
			{
				pixelFormat = ImagePixelFormat::Gray16;
			}
			else
			{
				pixelFormat = ImagePixelFormat::Gray8;
			}
		}
		else if (readInfo.colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
		{
			if (readInfo.bitDepth == 16)
			{
				pixelFormat = ImagePixelFormat::Gray16A16;
			}
			else
			{
				pixelFormat = ImagePixelFormat::Gray8A8;
			}
		}
		else if (readInfo.colorType == PNG_COLOR_TYPE_RGB)
		{
			pixelFormat = ImagePixelFormat::R8G8B8;
		}

		return ImageInfo{ Size{ readInfo.width, readInfo.height }, ImageFormat::PNG, pixelFormat, false };
	}

	////////////////////////////////////////////////////////////////
	//
	//	decode
	//
	////////////////////////////////////////////////////////////////

	Image PNGDecoder::decode(const FilePathView path, const PremultiplyAlpha premultiplyAlpha) const
	{
		return IImageDecoder::decode(path, premultiplyAlpha);
	}

	Image PNGDecoder::decode(std::unique_ptr<IReader> reader, const FilePathView, const PremultiplyAlpha premultiplyAlpha) const
	{
		LOG_SCOPED_DEBUG("PNGDecoder::decode()");

		if (not reader)
		{
			return{};
		}

		png_structp png_ptr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (not png_ptr)
		{
			return{};
		}

		ScopeExit cleanup_struct = [&]()
		{
			::png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		};

		png_infop info_ptr = ::png_create_info_struct(png_ptr);

		if (not info_ptr)
		{
			return{};
		}

		ScopeExit cleanup_info = [&]()
		{
			::png_destroy_info_struct(png_ptr, &info_ptr);
		};

		PNGReadInfo readInfo;

		if (not PreparePNGDecode(png_ptr, info_ptr, reader.get(), &readInfo))
		{
			return{};
		}

		Image image(readInfo.width, readInfo.height);

		Array<uint8*> rowPointers(readInfo.height);
		{
			const size_t stride = (static_cast<size_t>(readInfo.width) * readInfo.channels);
			uint8* pixels = image.dataAsUint8();

			for (size_t i = 0; i < readInfo.height; ++i)
			{
				rowPointers[i] = pixels;
				pixels += stride;
			}
		}

		if (not ReadPNGImage(png_ptr, rowPointers.data()))
		{
			return{};
		}

		LOG_TRACE(fmt::format("Image ({}x{}) decoded", readInfo.width, readInfo.height));

		if (premultiplyAlpha)
		{
			image.premultiplyAlpha();
		}

		return image;
	}

	////////////////////////////////////////////////////////////////
	//
	//	decodeGray16
	//
	////////////////////////////////////////////////////////////////

	Grid<uint16> PNGDecoder::decodeGray16(const FilePathView path) const
	{
		return IImageDecoder::decodeGray16(path);
	}

	Grid<uint16> PNGDecoder::decodeGray16(std::unique_ptr<IReader> reader, FilePathView) const
	{
		LOG_SCOPED_DEBUG("PNGDecoder::decodeGray16()");

		if (not reader)
		{
			return{};
		}

		png_structp png_ptr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (not png_ptr)
		{
			return{};
		}

		ScopeExit cleanup_struct = [&]()
		{
			::png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		};

		png_infop info_ptr = ::png_create_info_struct(png_ptr);

		if (not info_ptr)
		{
			return{};
		}

		ScopeExit cleanup_info = [&]()
		{
			::png_destroy_info_struct(png_ptr, &info_ptr);
		};

		PNGReadInfo readInfo;

		if (not PreparePNGGray16Decode(png_ptr, info_ptr, reader.get(), &readInfo))
		{
			return{};
		}

		Grid<uint16> image(readInfo.width, readInfo.height);

		Array<uint16*> rowPointers(readInfo.height);
		{
			uint16* pixels = image.data();

			for (size_t i = 0; i < readInfo.height; ++i)
			{
				rowPointers[i] = pixels;
				pixels += readInfo.width;
			}
		}

		if (not ReadPNGImage(png_ptr, static_cast<png_bytepp>(static_cast<void*>(rowPointers.data()))))
		{
			return{};
		}

		LOG_TRACE(fmt::format("Image ({}x{}) decoded", readInfo.width, readInfo.height));

		return image;
	}
}

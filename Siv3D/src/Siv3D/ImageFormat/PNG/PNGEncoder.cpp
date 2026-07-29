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

# include <Siv3D/ImageFormat/PNGEncoder.hpp>
# include <Siv3D/BinaryFileWriter.hpp>
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
		static void PngWriteCallbackIWriter(png_structp png_ptr, png_bytep src, png_size_t length)
		{
			IWriter* pWriter = static_cast<IWriter*>(::png_get_io_ptr(png_ptr));

			pWriter->write(src, length);
		}

		static void PngWriteCallbackBlob(png_structp png_ptr, png_bytep src, png_size_t length)
		{
			Blob* pBlob = static_cast<Blob*>(::png_get_io_ptr(png_ptr));

			pBlob->append(src, length);
		}

		[[nodiscard]] static bool IsValidPNGSize(const size_t width, const size_t height) noexcept
		{
			return ((0 < width)
				&& (0 < height)
				&& (width <= Image::MaxWidth)
				&& (height <= Image::MaxHeight)
				&& (width <= static_cast<size_t>(PNG_UINT_31_MAX))
				&& (height <= static_cast<size_t>(PNG_UINT_31_MAX)));
		}

		[[nodiscard]] static bool WritePNG(png_structp png_ptr, png_infop info_ptr,
			png_voidp ioPtr, png_rw_ptr writeCallback,
			const png_uint_32 width, const png_uint_32 height,
			const int bitDepth, const int colorType, const int filter,
			const uint8* pixels, const size_t bytesPerRow, const bool swapEndian)
		{
			if (setjmp(png_jmpbuf(png_ptr)))
			{
				return false;
			}

			::png_set_write_fn(png_ptr, ioPtr, writeCallback, nullptr);
			::png_set_IHDR(png_ptr, info_ptr, width, height, bitDepth, colorType,
				PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
			::png_set_filter(png_ptr, 0, filter);

			if (swapEndian)
			{
				// Siv3D supports little-endian platforms only.
				::png_set_swap(png_ptr);
			}

			::png_write_info(png_ptr, info_ptr);

			const uint8* row = pixels;

			for (png_uint_32 y = 0; y < height; ++y)
			{
				::png_write_row(png_ptr, row);
				row += bytesPerRow;
			}

			::png_write_end(png_ptr, info_ptr);

			return true;
		}
	}
	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView PNGEncoder::name() const noexcept
	{
		return U"PNG"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	imageFormat
	//
	////////////////////////////////////////////////////////////////

	ImageFormat PNGEncoder::imageFormat() const noexcept
	{
		return ImageFormat::PNG;
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> PNGEncoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"png" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool PNGEncoder::save(const Image& image, const FilePathView path) const
	{
		return save(image, path, PNGFilter::Default);
	}

	bool PNGEncoder::save(const Image& image, const FilePathView path, const PNGFilter filter) const
	{
		if ((not image)
			|| (not IsValidPNGSize(image.width(), image.height())))
		{
			LOG_FAIL("❌ PNGEncoder::save(): image size is invalid");
			return false;
		}

		BinaryFileWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		return encode(image, writer, filter);
	}

	bool PNGEncoder::save(const Grid<uint16>& image, const FilePathView path, const PNGFilter filter) const
	{
		if (not IsValidPNGSize(image.width(), image.height()))
		{
			LOG_FAIL("❌ PNGEncoder::save(): image size is invalid");
			return false;
		}

		BinaryFileWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		return encode(image, writer, filter);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encode
	//
	////////////////////////////////////////////////////////////////

	bool PNGEncoder::encode(const Image& image, IWriter& writer) const
	{
		return encode(image, writer, PNGFilter::Default);
	}

	bool PNGEncoder::encode(const Image& image, IWriter& writer, const PNGFilter filter) const
	{
		if ((not writer.isOpen())
			|| (not image)
			|| (not IsValidPNGSize(image.width(), image.height())))
		{
			return false;
		}

		const png_uint_32 width = static_cast<png_uint_32>(image.width());
		const png_uint_32 height = static_cast<png_uint_32>(image.height());

		png_structp png_ptr = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (not png_ptr)
		{
			return false;
		}

		png_infop info_ptr = ::png_create_info_struct(png_ptr);

		if (not info_ptr)
		{
			::png_destroy_write_struct(&png_ptr, nullptr);
			return false;
		}

		const bool succeeded = WritePNG(png_ptr, info_ptr,
			&writer, PngWriteCallbackIWriter,
			width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, FromEnum(filter),
			image.dataAsUint8(), image.bytesPerRow(), false);

		::png_destroy_write_struct(&png_ptr, &info_ptr);

		return succeeded;
	}

	bool PNGEncoder::encode(const Grid<uint16>& image, IWriter& writer, const PNGFilter filter) const
	{
		if ((not writer.isOpen())
			|| (not IsValidPNGSize(image.width(), image.height())))
		{
			return false;
		}

		const png_uint_32 width = static_cast<png_uint_32>(image.width());
		const png_uint_32 height = static_cast<png_uint_32>(image.height());

		png_structp png_ptr = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (not png_ptr)
		{
			return false;
		}

		png_infop info_ptr = ::png_create_info_struct(png_ptr);

		if (not info_ptr)
		{
			::png_destroy_write_struct(&png_ptr, nullptr);
			return false;
		}

		const bool succeeded = WritePNG(png_ptr, info_ptr,
			&writer, PngWriteCallbackIWriter,
			width, height, 16, PNG_COLOR_TYPE_GRAY, FromEnum(filter),
			static_cast<const uint8*>(static_cast<const void*>(image.data())),
			(static_cast<size_t>(width) * sizeof(uint16)), true);

		::png_destroy_write_struct(&png_ptr, &info_ptr);

		return succeeded;
	}

	Blob PNGEncoder::encode(const Image& image) const
	{
		return encode(image, PNGFilter::Default);
	}

	Blob PNGEncoder::encode(const Image& image, const PNGFilter filter) const
	{
		if ((not image)
			|| (not IsValidPNGSize(image.width(), image.height())))
		{
			return{};
		}

		const png_uint_32 width = static_cast<png_uint_32>(image.width());
		const png_uint_32 height = static_cast<png_uint_32>(image.height());

		png_structp png_ptr = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (not png_ptr)
		{
			return{};
		}

		png_infop info_ptr = ::png_create_info_struct(png_ptr);

		if (not info_ptr)
		{
			::png_destroy_write_struct(&png_ptr, nullptr);
			return{};
		}

		Blob blob;

		const bool succeeded = WritePNG(png_ptr, info_ptr,
			&blob, PngWriteCallbackBlob,
			width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, FromEnum(filter),
			image.dataAsUint8(), image.bytesPerRow(), false);

		::png_destroy_write_struct(&png_ptr, &info_ptr);

		if (not succeeded)
		{
			return{};
		}

		return blob;
	}

	Blob PNGEncoder::encode(const Grid<uint16>& image, const PNGFilter filter) const
	{
		if (not IsValidPNGSize(image.width(), image.height()))
		{
			return{};
		}

		const png_uint_32 width = static_cast<png_uint_32>(image.width());
		const png_uint_32 height = static_cast<png_uint_32>(image.height());

		png_structp png_ptr = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (not png_ptr)
		{
			return{};
		}

		png_infop info_ptr = ::png_create_info_struct(png_ptr);

		if (not info_ptr)
		{
			::png_destroy_write_struct(&png_ptr, nullptr);
			return{};
		}

		Blob blob;

		const bool succeeded = WritePNG(png_ptr, info_ptr,
			&blob, PngWriteCallbackBlob,
			width, height, 16, PNG_COLOR_TYPE_GRAY, FromEnum(filter),
			static_cast<const uint8*>(static_cast<const void*>(image.data())),
			(static_cast<size_t>(width) * sizeof(uint16)), true);

		::png_destroy_write_struct(&png_ptr, &info_ptr);

		if (not succeeded)
		{
			return{};
		}

		return blob;
	}
}

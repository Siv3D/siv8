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

// Special thanks to @Raclamusi for the BMP decoder implementation
// https://github.com/Siv3D/OpenSiv3D/pull/1207

# include <Siv3D/ImageFormat/BMPDecoder.hpp>
# include <Siv3D/IReader.hpp>
# include <Siv3D/EngineLog.hpp>
# include "BMPHeader.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView BMPDecoder::name() const noexcept
	{
		return U"BMP"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	imageFormat
	//
	////////////////////////////////////////////////////////////////

	ImageFormat BMPDecoder::imageFormat() const noexcept
	{
		return ImageFormat::BMP;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isHeader
	//
	////////////////////////////////////////////////////////////////

	bool BMPDecoder::isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept
	{
		static constexpr uint8 signature[] = { 0x42, 0x4d };

		return (std::memcmp(bytes, signature, sizeof(signature)) == 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> BMPDecoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"bmp" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getImageInfo
	//
	////////////////////////////////////////////////////////////////

	Optional<ImageInfo> BMPDecoder::getImageInfo(const FilePathView path) const
	{
		return IImageDecoder::getImageInfo(path);
	}

	Optional<ImageInfo> BMPDecoder::getImageInfo(const IReader& reader, const FilePathView) const
	{
		BMPHeader header;

		if (const size_t offset = offsetof(BMPHeader, biPlanes);
			offset != reader.lookahead(&header, offset))
		{
			LOG_FAIL("❌ BMPDecoder::getImageInfo(): Offset is invalid");
			return{};
		}

		const Size size{ header.biWidth, Abs(header.biHeight) };

		ImagePixelFormat pixelFormat = ImagePixelFormat::R8G8B8;

		if (header.biBitCount == 32)
		{
			pixelFormat = ImagePixelFormat::R8G8B8X8;
		}

		return ImageInfo{ size, ImageFormat::BMP, pixelFormat, false };
	}

	////////////////////////////////////////////////////////////////
	//
	//	decode
	//
	////////////////////////////////////////////////////////////////

	Image BMPDecoder::decode(const FilePathView path, const PremultiplyAlpha premultiplyAlpha) const
	{
		return IImageDecoder::decode(path, premultiplyAlpha);
	}

	Image BMPDecoder::decode(std::unique_ptr<IReader> reader, const FilePathView, const PremultiplyAlpha) const
	{
		LOG_SCOPED_DEBUG("BMPDecoder::decode()");

		BMPHeader header;

		if (not reader->read(header))
		{
			LOG_FAIL("❌ BMPDecoder::decode(): Failed to read header");
			return{};
		}

		if (header.bfType != 0x4d42)
		{
			LOG_FAIL("❌ BMPDecoder::decode(): BMPHeader::bfType is invalid");
			return{};
		}

		if (header.biSize != 40)
		{
			LOG_FAIL("❌ BMPDecoder::decode(): BMPHeader::biSize != 40");
			return{};
		}

		if (header.biPlanes != 1)
		{
			LOG_FAIL("❌ BMPDecoder::decode(): BMPHeader::biPlanes != 1");
			return{};
		}

		if (header.biCompression)
		{
			LOG_FAIL("❌ BMPDecoder::decode(): Compressed BMP is not supported");
			return{};
		}

		const int32 depth = header.biBitCount;

		if ((depth != 1)
			&& (depth != 4)
			&& (depth != 8)
			&& (depth != 16)
			&& (depth != 24)
			&& (depth != 32))
		{
			LOG_FAIL("❌ BMPDecoder::decode(): BMPHeader::biBitCount is invalid");
			return{};
		}

		const bool indexed = ((depth == 1) || (depth == 4) || (depth == 8));
		const bool reverse = (0 < header.biHeight);

		const int64 width64 = header.biWidth;
		const int64 height64 = reverse
			? static_cast<int64>(header.biHeight)
			: -static_cast<int64>(header.biHeight);

		if (not InRange<int64>(width64, 1, Image::MaxWidth)
			|| not InRange<int64>(height64, 1, Image::MaxHeight))
		{
			LOG_FAIL(fmt::format("BMPDecoder::decode(): Image size {}x{} is not supported", width64, height64));
			return{};
		}

		const int32 width = static_cast<int32>(width64);
		const int32 height = static_cast<int32>(height64);

		uint32 paletteEntries = 0;

		if (indexed)
		{
			const uint32 maxPaletteEntries = (1u << depth);

			paletteEntries = (header.biClrUsed == 0)
				? maxPaletteEntries
				: header.biClrUsed;

			if ((paletteEntries == 0) || (maxPaletteEntries < paletteEntries))
			{
				LOG_FAIL("❌ BMPDecoder::decode(): BMPHeader::biClrUsed is invalid");
				return{};
			}
		}

		const size_t paletteBytes = (static_cast<size_t>(paletteEntries) * 4);
		const size_t minimumPixelOffset = (sizeof(BMPHeader) + paletteBytes);

		if (header.bfOffBits < minimumPixelOffset)
		{
			LOG_FAIL("❌ BMPDecoder::decode(): BMPHeader::bfOffBits is invalid");
			return{};
		}

		Array<uint8> paletteOwner(paletteBytes);
		const uint8* const palette = paletteOwner.data();

		const auto readExact = [&](void* dst, const size_t size) -> bool
		{
			return (reader->read(dst, size) == static_cast<int64>(size));
		};

		if (paletteBytes)
		{
			if (not readExact(paletteOwner.data(), paletteBytes))
			{
				LOG_FAIL("❌ BMPDecoder::decode(): Failed to read palette");
				return{};
			}
		}

		if (header.bfOffBits != minimumPixelOffset)
		{
			if (not reader->setPos(header.bfOffBits))
			{
				LOG_FAIL("❌ BMPDecoder::decode(): Failed to seek to pixel data");
				return{};
			}
		}

		Image image(width, height);

		LOG_TRACE(fmt::format("BMPHeader::biBitCount: {}", depth));

		const auto setPaletteColor = [&](Color& dst, const size_t index) -> bool
		{
			if (paletteEntries <= index)
			{
				LOG_FAIL("❌ BMPDecoder::decode(): Palette index is out of range");
				return false;
			}

			const uint8* const src = (palette + (index * 4));
			dst.set(src[2], src[1], src[0]);
			return true;
		};

		switch (depth)
		{
		case 1:
			{
				const size_t rowSize = (((static_cast<size_t>(width) * depth + 31) / 32) * 4);
				const int32 lineStep = reverse ? -width : width;
				Color* pDstLine = image[reverse ? height - 1 : 0];

				Array<uint8> bufferOwner(rowSize);
				uint8* const buffer = bufferOwner.data();

				for (int32 y = 0; y < height; ++y)
				{
					if (not readExact(buffer, rowSize))
					{
						LOG_FAIL("❌ BMPDecoder::decode(): Failed to read pixel data");
						return{};
					}

					const uint8* tmp = buffer;
					Color* pDst = pDstLine;

					for (int32 x = 0; x < width; x += 8)
					{
						const int32 n = Min(8, (width - x));

						for (int32 i = 0; i < n; ++i)
						{
							const size_t index = ((*tmp >> (7 - i)) & 1);

							if (not setPaletteColor(*pDst, index))
							{
								return{};
							}

							++pDst;
						}

						++tmp;
					}

					pDstLine += lineStep;
				}

				break;
			}
		case 4:
			{
				const size_t rowSize = (((static_cast<size_t>(width) * depth + 31) / 32) * 4);
				const int32 lineStep = reverse ? -width : width;
				Color* pDstLine = image[reverse ? height - 1 : 0];

				Array<uint8> bufferOwner(rowSize);
				uint8* const buffer = bufferOwner.data();

				for (int32 y = 0; y < height; ++y)
				{
					if (not readExact(buffer, rowSize))
					{
						LOG_FAIL("❌ BMPDecoder::decode(): Failed to read pixel data");
						return{};
					}

					const uint8* tmp = buffer;
					Color* pDst = pDstLine;
					const int32 w = (width - 1);

					for (int32 x = 0; x < w; x += 2)
					{
						const size_t index1 = ((*tmp >> 4) & 0x0f);
						const size_t index2 = (*tmp & 0x0f);

						if (not setPaletteColor(*pDst, index1))
						{
							return{};
						}

						++pDst;

						if (not setPaletteColor(*pDst, index2))
						{
							return{};
						}

						++pDst;
						++tmp;
					}

					if (width & 1)
					{
						const size_t index = ((*tmp >> 4) & 0x0f);

						if (not setPaletteColor(*pDst, index))
						{
							return{};
						}

						++pDst;
					}

					pDstLine += lineStep;
				}

				break;
			}
		case 8:
			{
				const size_t rowSize = (((static_cast<size_t>(width) * depth + 31) / 32) * 4);
				const int32 lineStep = reverse ? -width : width;
				Color* pDstLine = image[reverse ? height - 1 : 0];

				Array<uint8> bufferOwner(rowSize);
				uint8* const buffer = bufferOwner.data();

				for (int32 y = 0; y < height; ++y)
				{
					if (not readExact(buffer, rowSize))
					{
						LOG_FAIL("❌ BMPDecoder::decode(): Failed to read pixel data");
						return{};
					}

					const uint8* tmp = buffer;
					const Color* const pDstEnd = (pDstLine + width);

					for (Color* pDst = pDstLine; pDst != pDstEnd; ++pDst)
					{
						const size_t index = static_cast<size_t>(*tmp++);

						if (not setPaletteColor(*pDst, index))
						{
							return{};
						}
					}

					pDstLine += lineStep;
				}

				break;
			}
		case 16:
			{
				const size_t rowSize = (((static_cast<size_t>(width) * depth + 31) / 32) * 4);
				const int32 lineStep = reverse ? -width : width;
				Color* pDstLine = image[reverse ? height - 1 : 0];

				Array<uint8> bufferOwner(rowSize);
				uint8* const buffer = bufferOwner.data();

				for (int32 y = 0; y < height; ++y)
				{
					if (not readExact(buffer, rowSize))
					{
						LOG_FAIL("❌ BMPDecoder::decode(): Failed to read pixel data");
						return{};
					}

					const uint8* pSrc = buffer;
					const Color* const pDstEnd = (pDstLine + width);

					for (Color* pDst = pDstLine; pDst != pDstEnd; ++pDst)
					{
						const uint16 pixel = static_cast<uint16>(pSrc[0] | (pSrc[1] << 8));

						const uint32 b = ((pixel & 0x001f) << 3);
						const uint32 g = ((pixel & 0x07e0) >> 2);
						const uint32 r = ((pixel & 0xf800) >> 7);

						pDst->set(r, g, b);

						pSrc += 2;
					}

					pDstLine += lineStep;
				}

				break;
			}
		case 24:
		case 32:
			{
				const size_t rowSize = (((static_cast<size_t>(width) * depth + 31) / 32) * 4);
				const size_t depthBytes = (depth / 8);
				const int32 lineStep = reverse ? -width : width;
				Color* pDstLine = image[reverse ? height - 1 : 0];

				Array<uint8> bufferOwner(rowSize);
				uint8* const buffer = bufferOwner.data();

				for (int32 y = 0; y < height; ++y)
				{
					if (not readExact(buffer, rowSize))
					{
						LOG_FAIL("❌ BMPDecoder::decode(): Failed to read pixel data");
						return{};
					}

					const uint8* pSrc = buffer;
					const Color* const pDstEnd = (pDstLine + width);

					for (Color* pDst = pDstLine; pDst != pDstEnd; ++pDst)
					{
						pDst->set(pSrc[2], pSrc[1], pSrc[0]);

						pSrc += depthBytes;
					}

					pDstLine += lineStep;
				}

				break;
			}
		default:
			{
				LOG_FAIL("❌ BMPDecoder::decode(): BMPHeader::biBitCount is invalid");
				return{};
			}
		}

		LOG_TRACE(fmt::format("Image ({}x{}) decoded", width, height));

		return image;
	}
}

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

# include <Siv3D/ImageFormat/BCnDecoder.hpp>
# include <Siv3D/ImageProcessing.hpp>
# include <Siv3D/IReader.hpp>
# include <Siv3D/BinaryFileReader.hpp>
# include <Siv3D/EngineLog.hpp>
# include <ThirdParty/bc7enc_rdo/rdo_bc_encoder.h>

namespace s3d
{
	namespace
	{
		struct DDSInfo
		{
			Size size;
			TextureFormat format;
			uint32 mipCount;
		};

		static bool ReadDDSBytes(IReader& reader, void* dst, const size_t size)
		{
			auto* out = static_cast<Byte*>(dst);
			int64 remaining = static_cast<int64>(size);
			while (remaining > 0)
			{
				const int64 read = reader.read(out, remaining);
				if ((read <= 0) || (remaining < read))
				{
					LOG_FAIL("❌ BCnDecoder: Failed to read DDS data");
					return false;
				}
				out += read;
				remaining -= read;
			}
			return true;
		}

		static TextureFormat ReadDDSFormat(IReader& reader, const uint32 fourCC, const bool sRGB)
		{
			switch (fourCC)
			{
			case PIXEL_FMT_DXT1:
				return (sRGB ? TextureFormat::BC1_RGBA_Unorm_SRGB : TextureFormat::BC1_RGBA_Unorm);
			case PIXEL_FMT_DXT5:
				return (sRGB ? TextureFormat::BC3_RGBA_Unorm_SRGB : TextureFormat::BC3_RGBA_Unorm);
			case PIXEL_FMT_FOURCC('B', 'C', '4', 'U'):
				return TextureFormat::BC4_R_Unorm;
			case PIXEL_FMT_FOURCC('B', 'C', '5', 'U'):
				return TextureFormat::BC5_RG_Unorm;
			case PIXEL_FMT_FOURCC('D', 'X', '1', '0'):
				break;
			default:
				LOG_FAIL(fmt::format("❌ BCnDecoder: Unsupported format {:#X}", fourCC));
				return TextureFormat::Unknown;
			}

			DDS_HEADER_DXT10 ext{};
			if (not ReadDDSBytes(reader, &ext, sizeof(ext)))
			{
				return TextureFormat::Unknown;
			}
			switch (ext.dxgiFormat)
			{
			case DXGI_FORMAT_BC4_UNORM:
				return TextureFormat::BC4_R_Unorm;
			case DXGI_FORMAT_BC5_UNORM:
				return TextureFormat::BC5_RG_Unorm;
			case DXGI_FORMAT_BC7_UNORM:
				return (sRGB ? TextureFormat::BC7_RGBA_Unorm_SRGB : TextureFormat::BC7_RGBA_Unorm);
			case DXGI_FORMAT_BC7_UNORM_SRGB:
				return TextureFormat::BC7_RGBA_Unorm_SRGB;
			default:
				LOG_FAIL(fmt::format("❌ BCnDecoder: Unsupported DXGI format {:#X}", static_cast<uint32>(ext.dxgiFormat)));
				return TextureFormat::Unknown;
			}
		}

		static Optional<DDSInfo> ReadDDSHeader(IReader& reader, const bool sRGB)
		{
			struct
			{
				uint32 magic;
				DDSURFACEDESC2 desc;
			} header{};
			static_assert(sizeof(header) == 128);

			if ((reader.setPos(0) != 0) || not ReadDDSBytes(reader, &header, sizeof(header)))
			{
				return{};
			}

			const auto& desc = header.desc;
			if ((header.magic != PIXEL_FMT_FOURCC('D', 'D', 'S', ' ')) || (desc.dwSize != sizeof(desc)))
			{
				LOG_FAIL("❌ BCnDecoder: Invalid DDS header");
				return{};
			}
			if (not InRange(desc.dwWidth, 1u, static_cast<uint32>(Image::MaxWidth))
				|| not InRange(desc.dwHeight, 1u, static_cast<uint32>(Image::MaxHeight)))
			{
				LOG_FAIL(fmt::format("BCnDecoder: Image size {}x{} is not supported", desc.dwWidth, desc.dwHeight));
				return{};
			}

			const uint32 mipCount = Max(desc.dwMipMapCount, 1u);
			if (ImageProcessing::CalculateMipmapLevel(desc.dwWidth, desc.dwHeight) < mipCount)
			{
				LOG_FAIL(fmt::format("BCnDecoder: Too many mip levels ({}) for {}x{}", mipCount, desc.dwWidth, desc.dwHeight));
				return{};
			}

			const TextureFormat format = ReadDDSFormat(reader, desc.ddpfPixelFormat.dwFourCC, sRGB);
			if (format == TextureFormat::Unknown)
			{
				return{};
			}
			return DDSInfo{ { desc.dwWidth, desc.dwHeight }, format, mipCount };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView BCnDecoder::name() const noexcept
	{
		return U"BCn"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	imageFormat
	//
	////////////////////////////////////////////////////////////////

	ImageFormat BCnDecoder::imageFormat() const noexcept
	{
		return ImageFormat::BCn;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isHeader
	//
	////////////////////////////////////////////////////////////////

	bool BCnDecoder::isHeader(const uint8(&bytes)[RequiredHeaderBytes]) const noexcept
	{
		static constexpr uint8 signature[] = { 'D', 'D', 'S', ' ' };

		if (std::memcmp(bytes, signature, sizeof(signature)) != 0)
		{
			return false;
		}

		struct Header
		{
			uint32 size;
			uint32 flags;
			uint32 height;
		} header;

		std::memcpy(&header, (bytes + 4), sizeof(Header));

		if (header.size != 124)
		{
			return false;
		}

		if (not InRange(header.height, 1u, 16384u))
		{
			return false;
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> BCnDecoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"dds" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getImageInfo
	//
	////////////////////////////////////////////////////////////////

	Optional<ImageInfo> BCnDecoder::getImageInfo(const FilePathView path) const
	{
		return IImageDecoder::getImageInfo(path);
	}

	Optional<ImageInfo> BCnDecoder::getImageInfo(const IReader& reader, const FilePathView) const
	{
		DDSURFACEDESC2 desc{};
		
		if (reader.lookahead(&desc, 4, sizeof(desc)) != sizeof(desc))
		{
			return{};
		}
		
		const Size size{ desc.dwWidth, desc.dwHeight };

		if (not((desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_DXT1) // BC1
			 || (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_DXT5)  // BC3
			 || (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_FOURCC('B', 'C', '4', 'U')) // BC4
			 || (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_FOURCC('B', 'C', '5', 'U')) // BC5
			 || (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_FOURCC('D', 'X', '1', '0')))) // BC4, BC5, BC7
		{
			return{};
		}

		return ImageInfo{ size, ImageFormat::BCn, ImagePixelFormat::R8G8B8A8, false };
	}

	////////////////////////////////////////////////////////////////
	//
	//	decode
	//
	////////////////////////////////////////////////////////////////

	Image BCnDecoder::decode(const FilePathView path, const PremultiplyAlpha premultiplyAlpha) const
	{
		return IImageDecoder::decode(path, premultiplyAlpha);
	}

	Image BCnDecoder::decode(std::unique_ptr<IReader> reader, const FilePathView, const PremultiplyAlpha) const
	{
		LOG_SCOPED_DEBUG("BCnDecoder::decode()");

		if (not reader)
		{
			return{};
		}

		const auto info = ReadDDSHeader(*reader, false);
		if (not info)
		{
			return{};
		}

		if ((reader->size() - reader->getPos()) < static_cast<int64>(info->format.packedDataSize(info->size)))
		{
			LOG_FAIL("❌ BCnDecoder::decode(): Incomplete base level");
			return{};
		}

		const uint32 width = info->size.x;
		const uint32 height = info->size.y;
		Image image{ width, height };

		const uint32 xBlocks = ((width + 3) / 4);
		const uint32 yBlocks = ((height + 3) / 4);

		if (info->format == TextureFormat::BC1_RGBA_Unorm) // BC1
		{
			constexpr uint32 BlockSize = 8;
			const size_t lineSize = (xBlocks * BlockSize);
			Array<Byte> lineBuffer(lineSize);

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				if (not ReadDDSBytes(*reader, lineBuffer.data(), lineSize))
				{
					return{};
				}

				for (uint32 bx = 0; bx < xBlocks; ++bx)
				{
					const void* pBlock = (lineBuffer.data() + (bx * BlockSize));

					utils::color_quad_u8 unpackedPixels[16];
					rgbcx::unpack_bc1(pBlock, unpackedPixels, true);		
					{
						const uint32 dstBaseY = (by * 4);
						const uint32 dstBaseX = (bx * 4);
						const utils::color_quad_u8* pSrc = unpackedPixels;

						for (uint32 y = 0; y < 4; ++y)
						{
							for (uint32 x = 0; x < 4; ++x)
							{
								const uint32 dstX = (dstBaseX + x);
								const uint32 dstY = (dstBaseY + y);

								if ((width <= dstX) || (height <= dstY))
								{
									++pSrc;
									continue;
								}

								image[dstY][dstX].set(pSrc->r, pSrc->g, pSrc->b, pSrc->a);
								++pSrc;
							}
						}
					}
				}
			}
		}
		else if (info->format == TextureFormat::BC3_RGBA_Unorm) // BC3
		{
			constexpr uint32 BlockSize = 16;
			const size_t lineSize = (xBlocks * BlockSize);
			Array<Byte> lineBuffer(lineSize);

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				if (not ReadDDSBytes(*reader, lineBuffer.data(), lineSize))
				{
					return{};
				}

				for (uint32 bx = 0; bx < xBlocks; ++bx)
				{
					const void* pBlock = (lineBuffer.data() + (bx * BlockSize));

					utils::color_quad_u8 unpackedPixels[16];
					rgbcx::unpack_bc3(pBlock, unpackedPixels);
					{
						const uint32 dstBaseY = (by * 4);
						const uint32 dstBaseX = (bx * 4);
						const utils::color_quad_u8* pSrc = unpackedPixels;

						for (uint32 y = 0; y < 4; ++y)
						{
							for (uint32 x = 0; x < 4; ++x)
							{
								const uint32 dstX = (dstBaseX + x);
								const uint32 dstY = (dstBaseY + y);

								if ((width <= dstX) || (height <= dstY))
								{
									++pSrc;
									continue;
								}

								image[dstY][dstX].set(pSrc->r, pSrc->g, pSrc->b, pSrc->a);
								++pSrc;
							}
						}
					}
				}
			}
		}
		else if (info->format == TextureFormat::BC4_R_Unorm)
		{
			constexpr uint32 BlockSize = 8;
			const size_t lineSize = (xBlocks * BlockSize);
			Array<Byte> lineBuffer(lineSize);

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				if (not ReadDDSBytes(*reader, lineBuffer.data(), lineSize))
				{
					return{};
				}

				for (uint32 bx = 0; bx < xBlocks; ++bx)
				{
					const void* pBlock = (lineBuffer.data() + (bx * BlockSize));

					uint8 unpackedPixels[16];
					rgbcx::unpack_bc4(pBlock, unpackedPixels, 1);
					{
						const uint32 dstBaseY = (by * 4);
						const uint32 dstBaseX = (bx * 4);
						const uint8* pSrc = unpackedPixels;

						for (uint32 y = 0; y < 4; ++y)
						{
							for (uint32 x = 0; x < 4; ++x)
							{
								const uint32 dstX = (dstBaseX + x);
								const uint32 dstY = (dstBaseY + y);

								if ((width <= dstX) || (height <= dstY))
								{
									++pSrc;
									continue;
								}

								image[dstY][dstX].set(*pSrc, *pSrc, *pSrc, 255);
								++pSrc;
							}
						}
					}
				}
			}
		}
		else if (info->format == TextureFormat::BC5_RG_Unorm)
		{
			constexpr uint32 BlockSize = 16;
			const size_t lineSize = (xBlocks * BlockSize);
			Array<Byte> lineBuffer(lineSize);

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				if (not ReadDDSBytes(*reader, lineBuffer.data(), lineSize))
				{
					return{};
				}

				for (uint32 bx = 0; bx < xBlocks; ++bx)
				{
					const void* pBlock = (lineBuffer.data() + (bx * BlockSize));

					uint8 unpackedPixels[32];
					rgbcx::unpack_bc5(pBlock, unpackedPixels, 0, 1, 2);
					{
						const uint32 dstBaseY = (by * 4);
						const uint32 dstBaseX = (bx * 4);
						const uint8* pSrc = unpackedPixels;

						for (uint32 y = 0; y < 4; ++y)
						{
							for (uint32 x = 0; x < 4; ++x)
							{
								const uint32 dstX = (dstBaseX + x);
								const uint32 dstY = (dstBaseY + y);

								if ((width <= dstX) || (height <= dstY))
								{
									pSrc += 2;
									continue;
								}

								image[dstY][dstX].set(pSrc[0], pSrc[1], 0, 255);
								pSrc += 2;
							}
						}
					}
				}
			}
		}
		else if ((info->format == TextureFormat::BC7_RGBA_Unorm) || (info->format == TextureFormat::BC7_RGBA_Unorm_SRGB))
		{
			constexpr uint32 BlockSize = 16;
			const size_t lineSize = (xBlocks * BlockSize);
			Array<Byte> lineBuffer(lineSize);

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				if (not ReadDDSBytes(*reader, lineBuffer.data(), lineSize))
				{
					return{};
				}

				for (uint32 bx = 0; bx < xBlocks; ++bx)
				{
					const void* pBlock = (lineBuffer.data() + (bx * BlockSize));

					bc7decomp::color_rgba unpackedPixels[16];
					bc7decomp::unpack_bc7(pBlock, unpackedPixels);
					{
						const uint32 dstBaseY = (by * 4);
						const uint32 dstBaseX = (bx * 4);
						const bc7decomp::color_rgba* pSrc = unpackedPixels;

						for (uint32 y = 0; y < 4; ++y)
						{
							for (uint32 x = 0; x < 4; ++x)
							{
								const uint32 dstX = (dstBaseX + x);
								const uint32 dstY = (dstBaseY + y);

								if ((width <= dstX) || (height <= dstY))
								{
									++pSrc;
									continue;
								}

								image[dstY][dstX].set(pSrc->r, pSrc->g, pSrc->b, pSrc->a);
								++pSrc;
							}
						}
					}
				}
			}
		}
		else
		{
			LOG_FAIL("❌ BCnDecoder::decode(): Unsupported format");
			return{};
		}

		LOG_TRACE(fmt::format("Image ({}x{}) decoded", width, height));

		return image;
	}

	////////////////////////////////////////////////////////////////
	//
	//	decodeNative
	//
	////////////////////////////////////////////////////////////////

	BCnData BCnDecoder::decodeNative(const FilePathView path, const bool sRGB) const
	{
		std::unique_ptr<BinaryFileReader> reader = std::make_unique<BinaryFileReader>(path);

		if (not reader->isOpen())
		{
			return{};
		}

		return decodeNative(std::move(reader), sRGB, path);
	}

	BCnData BCnDecoder::decodeNative(std::unique_ptr<IReader> reader, const bool sRGB, const FilePathView) const
	{
		LOG_SCOPED_DEBUG("BCnDecoder::decodeNative()");

		if (not reader)
		{
			return{};
		}

		const auto info = ReadDDSHeader(*reader, sRGB);
		if (not info)
		{
			return{};
		}

		BCnData bcnData;
		bcnData.size = info->size;
		bcnData.format = info->format;
		bcnData.textures.reserve(info->mipCount);
		int64 remaining = (reader->size() - reader->getPos());
		for (uint32 level = 0; level < info->mipCount; ++level)
		{
			const Size size = ImageProcessing::CalculateMipmapSize(info->size, level);
			const size_t bytes = info->format.packedDataSize(size);
			if (remaining < static_cast<int64>(bytes))
			{
				LOG_FAIL(fmt::format("❌ BCnDecoder::decodeNative(): Incomplete mip level {}", level));
				return{};
			}
			Blob blob{ bytes };
			if (not ReadDDSBytes(*reader, blob.data(), bytes))
			{
				return{};
			}
			bcnData.textures.push_back(std::move(blob));
			remaining -= static_cast<int64>(bytes);
		}

		LOG_TRACE(fmt::format("Image ({}x{}, Mips: {}) decoded", info->size.x, info->size.y, info->mipCount));

		return bcnData;
	}
}

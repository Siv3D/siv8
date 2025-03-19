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

# include <Siv3D/ImageFormat/BCnDecoder.hpp>
# include <Siv3D/IReader.hpp>
# include <Siv3D/BinaryReader.hpp>
# include <Siv3D/EngineLog.hpp>
# include <ThirdParty/bc7enc_rdo/rdo_bc_encoder.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr TextureFormat ToTextureFormat(const DXGI_FORMAT format) noexcept
		{
			switch (format)
			{
			case DXGI_FORMAT_BC1_UNORM:
				return TextureFormat::BC1_RGBA_Unorm;
			case DXGI_FORMAT_BC3_UNORM:
				return TextureFormat::BC3_RGBA_Unorm;
			case DXGI_FORMAT_BC4_UNORM:
				return TextureFormat::BC4_R_Unorm;
			case DXGI_FORMAT_BC5_UNORM:
				return TextureFormat::BC5_RG_Unorm;
			case DXGI_FORMAT_BC7_UNORM:
				return TextureFormat::BC7_RGBA_Unorm;
			default:
				return TextureFormat::Unknown;
			}
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

	bool BCnDecoder::isHeader(const uint8(&bytes)[16]) const noexcept
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

	const Array<String>& BCnDecoder::possibleExtensions() const
	{
		static const Array<String> extensions{ U"dds" };

		return extensions;
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

	Optional<ImageInfo> BCnDecoder::getImageInfo(IReader& reader, const FilePathView) const
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

	Image BCnDecoder::decode(IReader& reader, const FilePathView, const PremultiplyAlpha) const
	{
		LOG_SCOPED_DEBUG("BCnDecoder::decode()");

		DDSURFACEDESC2 desc{};

		if (reader.read(&desc, 4, sizeof(desc)) != sizeof(desc))
		{
			LOG_FAIL("❌ BCnDecoder::decode(): Failed to read header");
			return{};
		}

		if (desc.dwSize != 124)
		{
			LOG_FAIL("❌ BCnDecoder::decode(): Invalid header size");
			return{};
		}

		const bool hasDX10Header = (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_FOURCC('D', 'X', '1', '0'));

		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;

		if (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_DXT1) // BC1
		{
			dxgiFormat = DXGI_FORMAT_BC1_UNORM;
		}
		else if (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_DXT5) // BC3
		{
			dxgiFormat = DXGI_FORMAT_BC3_UNORM;
		}
		else if (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_FOURCC('B', 'C', '4', 'U')) // BC4
		{
			dxgiFormat = DXGI_FORMAT_BC4_UNORM;
		}
		else if (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_FOURCC('B', 'C', '5', 'U')) // BC5
		{
			dxgiFormat = DXGI_FORMAT_BC5_UNORM;
		}
		else if (not hasDX10Header)
		{
			LOG_FAIL(fmt::format("❌ BCnDecoder::decode(): Unsupported format {:#X}", desc.ddpfPixelFormat.dwFourCC));
			return{};
		}

		const uint32 width = desc.dwWidth;
		const uint32 height = desc.dwHeight;

		if (not InRange<int32>(width, 1, Image::MaxWidth)
			|| not InRange<int32>(height, 1, Image::MaxHeight))
		{
			LOG_FAIL(fmt::format("BCnDecoder::decode(): Image size {}x{} is not supported", width, height));
			return{};
		}

		DDS_HEADER_DXT10 hdr10{};

		if (hasDX10Header)
		{
			if (reader.read(&hdr10, sizeof(hdr10)) != sizeof(hdr10))
			{
				LOG_FAIL("❌ BCnDecoder::decode(): Failed to read DXT10 header");
				return{};
			}

			if (hdr10.dxgiFormat == DXGI_FORMAT_BC4_UNORM) // BC4
			{
				dxgiFormat = DXGI_FORMAT_BC4_UNORM;
			}
			else if (hdr10.dxgiFormat == DXGI_FORMAT_BC5_UNORM) // BC5
			{
				dxgiFormat = DXGI_FORMAT_BC5_UNORM;
			}
			else if ((hdr10.dxgiFormat == DXGI_FORMAT_BC7_UNORM)
				|| (hdr10.dxgiFormat == DXGI_FORMAT_BC7_UNORM_SRGB)) // BC7
			{
				dxgiFormat = DXGI_FORMAT_BC7_UNORM;
			}
			else if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
			{
				LOG_FAIL(fmt::format("❌ BCnDecoder::decode(): Unsupported DXGI format {:#X}", static_cast<uint32>(hdr10.dxgiFormat)));
				return{};
			}
		}

		Image image{ width, height };

		const uint32 xBlocks = ((width + 3) / 4);
		const uint32 yBlocks = ((height + 3) / 4);

		if (dxgiFormat == DXGI_FORMAT_BC1_UNORM) // BC1
		{
			constexpr uint32 BlockSize = 8;
			const size_t lineSize = (xBlocks * BlockSize);
			Array<Byte> lineBuffer(lineSize);

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				reader.read(lineBuffer.data(), lineSize);

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
		else if (dxgiFormat == DXGI_FORMAT_BC3_UNORM) // BC3
		{
			constexpr uint32 BlockSize = 16;
			const size_t lineSize = (xBlocks * BlockSize);
			Array<Byte> lineBuffer(lineSize);

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				reader.read(lineBuffer.data(), lineSize);

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
		else if (dxgiFormat == DXGI_FORMAT_BC4_UNORM)
		{
			constexpr uint32 BlockSize = 8;
			const size_t lineSize = (xBlocks * BlockSize);
			Array<Byte> lineBuffer(lineSize);

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				reader.read(lineBuffer.data(), lineSize);

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
		else if (dxgiFormat == DXGI_FORMAT_BC5_UNORM)
		{
			constexpr uint32 BlockSize = 16;
			const size_t lineSize = (xBlocks * BlockSize);
			Array<Byte> lineBuffer(lineSize);

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				reader.read(lineBuffer.data(), lineSize);

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
		else if (dxgiFormat == DXGI_FORMAT_BC7_UNORM)
		{
			constexpr uint32 BlockSize = 16;
			const size_t lineSize = (xBlocks * BlockSize);
			Array<Byte> lineBuffer(lineSize);

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				reader.read(lineBuffer.data(), lineSize);

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

	BCnData BCnDecoder::decodeNative(const FilePathView path) const
	{
		BinaryReader reader{ path };

		if (not reader)
		{
			return{};
		}

		return decodeNative(reader, path);
	}

	BCnData BCnDecoder::decodeNative(IReader& reader, const FilePathView) const
	{
		LOG_SCOPED_DEBUG("BCnDecoder::decodeNative()");

		DDSURFACEDESC2 desc{};

		if (reader.read(&desc, 4, sizeof(desc)) != sizeof(desc))
		{
			LOG_FAIL("❌ BCnDecoder::decodeNative(): Failed to read header");
			return{};
		}

		if (desc.dwSize != 124)
		{
			LOG_FAIL("❌ BCnDecoder::decodeNative(): Invalid header size");
			return{};
		}

		const bool hasDX10Header = (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_FOURCC('D', 'X', '1', '0'));

		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;

		if (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_DXT1) // BC1
		{
			dxgiFormat = DXGI_FORMAT_BC1_UNORM;
		}
		else if (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_DXT5) // BC3
		{
			dxgiFormat = DXGI_FORMAT_BC3_UNORM;
		}
		else if (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_FOURCC('B', 'C', '4', 'U')) // BC4
		{
			dxgiFormat = DXGI_FORMAT_BC4_UNORM;
		}
		else if (desc.ddpfPixelFormat.dwFourCC == PIXEL_FMT_FOURCC('B', 'C', '5', 'U')) // BC5
		{
			dxgiFormat = DXGI_FORMAT_BC5_UNORM;
		}
		else if (not hasDX10Header)
		{
			LOG_FAIL(fmt::format("❌ BCnDecoder::decodeNative(): Unsupported format {:#X}", desc.ddpfPixelFormat.dwFourCC));
			return{};
		}

		const uint32 width = desc.dwWidth;
		const uint32 height = desc.dwHeight;
		const uint32 mipCount = Max(desc.dwMipMapCount, 1u);

		if (not InRange<int32>(width, 1, Image::MaxWidth)
			|| not InRange<int32>(height, 1, Image::MaxHeight))
		{
			LOG_FAIL(fmt::format("BCnDecoder::decodeNative(): Image size {}x{} is not supported", width, height));
			return{};
		}

		DDS_HEADER_DXT10 hdr10{};

		if (hasDX10Header)
		{
			if (reader.read(&hdr10, sizeof(hdr10)) != sizeof(hdr10))
			{
				LOG_FAIL("❌ BCnDecoder::decodeNative(): Failed to read DXT10 header");
				return{};
			}

			if (hdr10.dxgiFormat == DXGI_FORMAT_BC4_UNORM) // BC4
			{
				dxgiFormat = DXGI_FORMAT_BC4_UNORM;
			}
			else if (hdr10.dxgiFormat == DXGI_FORMAT_BC5_UNORM) // BC5
			{
				dxgiFormat = DXGI_FORMAT_BC5_UNORM;
			}
			else if ((hdr10.dxgiFormat == DXGI_FORMAT_BC7_UNORM)
				|| (hdr10.dxgiFormat == DXGI_FORMAT_BC7_UNORM_SRGB)) // BC7
			{
				dxgiFormat = DXGI_FORMAT_BC7_UNORM;
			}
			else if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
			{
				LOG_FAIL(fmt::format("❌ BCnDecoder::decode(): Unsupported DXGI format {:#X}", static_cast<uint32>(hdr10.dxgiFormat)));
				return{};
			}
		}

		const uint32 blockSize = (((dxgiFormat == DXGI_FORMAT_BC1_UNORM) || (dxgiFormat == DXGI_FORMAT_BC4_UNORM)) ? 8 : 16);

		BCnData bcnData;
		bcnData.size = { width, height };
		bcnData.format = ToTextureFormat(dxgiFormat);

		for (uint32 i = 0; i < mipCount; ++i)
		{
			const uint32 mipWidth = Max(1u, (width >> i));
			const uint32 mipHeight = Max(1u, (height >> i));
			const uint32 xBlocks = ((mipWidth + 3) / 4);
			const uint32 yBlocks = ((mipHeight + 3) / 4);
			const size_t textureSize = (xBlocks * yBlocks * blockSize);

			Blob blob{ textureSize };
			if (reader.read(blob.data(), textureSize) != static_cast<int64>(textureSize))
			{
				LOG_FAIL("❌ BCnDecoder::decodeNative(): Failed to read texture data");
				return{};
			}

			bcnData.textures.push_back(std::move(blob));
		}

		LOG_TRACE(fmt::format("Image ({}x{}, Mips: {}) decoded", width, height, mipCount));

		return bcnData;
	}
}

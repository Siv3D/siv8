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

# include <Siv3D/ImageFormat/BCnEncoder.hpp>
# include <Siv3D/BinaryFileWriter.hpp>
# include <Siv3D/MemoryWriter.hpp>
# include <Siv3D/ImageProcessing.hpp>
# include <Siv3D/EngineLog.hpp>
# include <ThirdParty/bc7enc_rdo/rdo_bc_encoder.h>

namespace s3d
{
	namespace
	{
		constexpr uint32 FourCC_DX10 = PIXEL_FMT_FOURCC('D', 'X', '1', '0');

		static bool WriteDDSHeader(IWriter& writer, const uint32 fourCC, const uint32 width, const uint32 height,
			const uint32 blockBytes, const uint32 mipCount)
		{
			constexpr uint8 FourCC[4] = { 'D', 'D', 'S', ' ' };
			if (not writer.write(FourCC))
			{
				return false;
			}

			DDSURFACEDESC2 desc{};
			desc.dwSize = sizeof(desc);
			desc.dwFlags = (DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_CAPS | DDSD_MIPMAPCOUNT | DDSD_LINEARSIZE);
			desc.dwHeight = height;
			desc.dwWidth = width;
			desc.lPitch = ((width / 4) * (height / 4) * blockBytes);
			desc.dwBackBufferCount = 1;
			desc.dwMipMapCount = mipCount;
			desc.ddpfPixelFormat.dwSize = sizeof(desc.ddpfPixelFormat);
			desc.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			desc.ddpfPixelFormat.dwFourCC = fourCC;
			desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
			if (not writer.write(desc))
			{
				return false;
			}

			if (fourCC != FourCC_DX10)
			{
				return true;
			}

			DDS_HEADER_DXT10 header{};
			header.dxgiFormat = DXGI_FORMAT_BC7_UNORM;
			header.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
			header.miscFlag = 0;
			header.arraySize = 1;
			header.miscFlags2 = 0;
			return writer.write(header);
		}

		template <uint32 BlockBytes, class CompressBlock>
		static bool EncodeBlocks(IWriter& writer, const Image& image, const CompressBlock& compressBlock)
		{
			const uint32 width = image.width();
			const uint32 height = image.height();

			const uint32 paddedWidth = ((width + 3) & ~3);
			const uint32 paddedHeight = ((height + 3) & ~3);

			const uint32 xBlocks = (paddedWidth / 4);
			const uint32 yBlocks = (paddedHeight / 4);

			uint8 pixels[4 * 4 * sizeof(Color)]{};
			std::array<Byte, BlockBytes> block{};

			for (uint32 by = 0; by < yBlocks; ++by)
			{
				for (uint32 bx = 0; bx < xBlocks; ++bx)
				{
					const uint32 dstBaseY = (by * 4);
					const uint32 dstBaseX = (bx * 4);
					uint8* pDst = pixels;

					for (uint32 y = 0; y < 4; ++y)
					{
						if ((height <= (dstBaseY + y)))
						{
							std::memcpy(pDst, (pDst - (4 * sizeof(Color))), (4 * sizeof(Color)));
							pDst += (4 * sizeof(Color));
							continue;
						}

						for (uint32 x = 0; x < 4; ++x)
						{
							if ((width <= (dstBaseX + x)))
							{
								std::memcpy(pDst, (pDst - sizeof(Color)), sizeof(Color));
								pDst += sizeof(Color);
								continue;
							}

							std::memcpy(pDst, &image[dstBaseY + y][dstBaseX + x], sizeof(Color));
							pDst += sizeof(Color);
						}
					}

					compressBlock(block.data(), pixels);

					if (not writer.write(block))
					{
						return false;
					}
				}
			}

			return true;
		}

		template <uint32 BlockBytes, class CompressBlock>
		static bool EncodeDDS(const Image& image, IWriter& writer, const GenerateMipmaps generateMipmaps,
			const uint32 fourCC, const CompressBlock& compressBlock)
		{
			if ((not image) || (not writer.isOpen()))
			{
				return false;
			}

			const uint32 width = image.width();
			const uint32 height = image.height();
			if (((width % 4) != 0) || ((height % 4) != 0))
			{
				return false;
			}

			const uint32 mipCount = (generateMipmaps ? ImageProcessing::CalculateMipmapLevel(width, height) : 1);
			if (not WriteDDSHeader(writer, fourCC, width, height, BlockBytes, mipCount))
			{
				return false;
			}

			if (not EncodeBlocks<BlockBytes>(writer, image, compressBlock))
			{
				return false;
			}

			Image mipImage;
			for (uint32 mip = 1; mip < mipCount; ++mip)
			{
				const Size size = ImageProcessing::CalculateMipmapSize(image.size(), mip);
				mipImage = ImageProcessing::Resize(((mip == 1) ? image : mipImage), size);

				if (not EncodeBlocks<BlockBytes>(writer, mipImage, compressBlock))
				{
					return false;
				}
			}

			return true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	BCnEncoder::BCnEncoder()
	{
		static bool s_initialized = false;

		if (not s_initialized)
		{
			s_initialized = true;
			rgbcx::init(rgbcx::bc1_approx_mode::cBC1Ideal);
			bc7enc_compress_block_init();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView BCnEncoder::name() const noexcept
	{
		return U"BCn"_sv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	imageFormat
	//
	////////////////////////////////////////////////////////////////

	ImageFormat BCnEncoder::imageFormat() const noexcept
	{
		return ImageFormat::BCn;
	}

	////////////////////////////////////////////////////////////////
	//
	//	possibleExtensions
	//
	////////////////////////////////////////////////////////////////

	std::span<const StringView> BCnEncoder::possibleExtensions() const noexcept
	{
		static constexpr std::array<StringView, 1> Extensions = { U"dds" };
		return Extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::save(const Image& image, const FilePathView path) const
	{
		if (not image)
		{
			LOG_FAIL("❌ BCnEncoder::save(): image is empty");
			return false;
		}

		BinaryFileWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		return encode(image, writer);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encode
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::encode(const Image& image, IWriter& writer) const
	{
		return encodeBC3(image, writer);
	}

	Blob BCnEncoder::encode(const Image& image) const
	{
		return encodeBC3(image);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeBC1
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::encodeBC1(const Image& image, IWriter& writer, const GenerateMipmaps generateMipmaps) const
	{
		return EncodeDDS<8>(image, writer, generateMipmaps, PIXEL_FMT_DXT1, [](Byte* block, const uint8* pixels)
		{
			rgbcx::encode_bc1(rgbcx::MAX_LEVEL, block, pixels, true, false);
		});
	}

	Blob BCnEncoder::encodeBC1(const Image& image, const GenerateMipmaps generateMipmaps) const
	{
		MemoryWriter writer;
	
		if (not encodeBC1(image, writer, generateMipmaps))
		{
			return{};
		}
		
		return writer.extractBlob();
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeBC3
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::encodeBC3(const Image& image, IWriter& writer, const GenerateMipmaps generateMipmaps) const
	{
		return EncodeDDS<16>(image, writer, generateMipmaps, PIXEL_FMT_DXT5, [](Byte* block, const uint8* pixels)
		{
			rgbcx::encode_bc3_hq(rgbcx::MAX_LEVEL, block, pixels);
		});
	}

	Blob BCnEncoder::encodeBC3(const Image& image, const GenerateMipmaps generateMipmaps) const
	{
		MemoryWriter writer;

		if (not encodeBC3(image, writer, generateMipmaps))
		{
			return{};
		}

		return writer.extractBlob();
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeBC4
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::encodeBC4(const Image& image, IWriter& writer, const GenerateMipmaps generateMipmaps) const
	{
		return EncodeDDS<8>(image, writer, generateMipmaps, PIXEL_FMT_FOURCC('B', 'C', '4', 'U'), [](Byte* block, const uint8* pixels)
		{
			rgbcx::encode_bc4_hq(block, pixels);
		});
	}

	Blob BCnEncoder::encodeBC4(const Image& image, const GenerateMipmaps generateMipmaps) const
	{
		MemoryWriter writer;

		if (not encodeBC4(image, writer, generateMipmaps))
		{
			return{};
		}

		return writer.extractBlob();
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeBC5
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::encodeBC5(const Image& image, IWriter& writer, const GenerateMipmaps generateMipmaps) const
	{
		return EncodeDDS<16>(image, writer, generateMipmaps, PIXEL_FMT_FOURCC('B', 'C', '5', 'U'), [](Byte* block, const uint8* pixels)
		{
			rgbcx::encode_bc5_hq(block, pixels);
		});
	}

	Blob BCnEncoder::encodeBC5(const Image& image, const GenerateMipmaps generateMipmaps) const
	{
		MemoryWriter writer;
		
		if (not encodeBC5(image, writer, generateMipmaps))
		{
			return{};
		}
		
		return writer.extractBlob();
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeBC7
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::encodeBC7(const Image& image, IWriter& writer, const GenerateMipmaps generateMipmaps) const
	{
		bc7enc_compress_block_params bc7Params{};
		bc7enc_compress_block_params_init(&bc7Params);

		return EncodeDDS<16>(image, writer, generateMipmaps, FourCC_DX10, [&bc7Params](Byte* block, const uint8* pixels)
		{
			bc7enc_compress_block(block, pixels, &bc7Params);
		});
	}

	Blob BCnEncoder::encodeBC7(const Image& image, const GenerateMipmaps generateMipmaps) const
	{
		MemoryWriter writer;

		if (not encodeBC7(image, writer, generateMipmaps))
		{
			return{};
		}

		return writer.extractBlob();
	}
}

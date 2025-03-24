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

# include <Siv3D/ImageFormat/BCnEncoder.hpp>
# include <Siv3D/BinaryWriter.hpp>
# include <Siv3D/MemoryWriter.hpp>
# include <Siv3D/ImageProcessing.hpp>
# include <Siv3D/EngineLog.hpp>
# include <ThirdParty/bc7enc_rdo/rdo_bc_encoder.h>

namespace s3d
{
	namespace
	{
		static void WriteDDSHeader(IWriter& writer, const pixel_format format, const uint32 width, const uint32 height, const uint32 pixelFormatBpp, const uint32 mipCount)
		{
			constexpr uint8 FourCC[4] = { 'D', 'D', 'S', ' ' };
			writer.write(FourCC);

			DDSURFACEDESC2 desc{};
			desc.dwSize = sizeof(desc);
			desc.dwFlags = (DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_CAPS | DDSD_MIPMAPCOUNT | DDSD_LINEARSIZE);
			desc.dwHeight = height;
			desc.dwWidth = width;
			desc.lPitch = ((width * height * pixelFormatBpp) / 8);
			desc.dwBackBufferCount = 1;
			desc.dwMipMapCount = mipCount;
			desc.ddpfPixelFormat.dwSize = sizeof(desc.ddpfPixelFormat);
			desc.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
			desc.ddpfPixelFormat.dwFourCC = format;
			desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
			writer.write(desc);
		}

		static void WriteDX10Header(IWriter& writer, const DXGI_FORMAT dxgiFormat)
		{
			DDS_HEADER_DXT10 header{};
			header.dxgiFormat = dxgiFormat;
			header.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
			header.miscFlag = 0;
			header.arraySize = 1;
			header.miscFlags2 = 0;
			writer.write(header);
		}

		static void EncodeBC1(IWriter& writer, const Image& image)
		{
			const uint32 width = image.width();
			const uint32 height = image.height();

			const uint32 paddedWidth = ((width + 3) & ~3);
			const uint32 paddedHeight = ((height + 3) & ~3);

			const uint32 xBlocks = (paddedWidth / 4);
			const uint32 yBlocks = (paddedHeight / 4);

			uint8 pixels[16 * 4 * 4]{};
			std::array<Byte, 8> block{};

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

					rgbcx::encode_bc1(rgbcx::MAX_LEVEL, block.data(), pixels, true, false);
					writer.write(block);
				}
			}
		}

		static void EncodeBC3(IWriter& writer, const Image& image)
		{
			const uint32 width = image.width();
			const uint32 height = image.height();

			const uint32 paddedWidth = ((width + 3) & ~3);
			const uint32 paddedHeight = ((height + 3) & ~3);

			const uint32 xBlocks = (paddedWidth / 4);
			const uint32 yBlocks = (paddedHeight / 4);

			uint8 pixels[16 * 4 * 4]{};
			std::array<Byte, 16> block{};

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

					rgbcx::encode_bc3_hq(rgbcx::MAX_LEVEL, block.data(), pixels);
					writer.write(block);
				}
			}
		}

		static void EncodeBC4(IWriter& writer, const Image& image)
		{
			const uint32 width = image.width();
			const uint32 height = image.height();

			const uint32 paddedWidth = ((width + 3) & ~3);
			const uint32 paddedHeight = ((height + 3) & ~3);

			const uint32 xBlocks = (paddedWidth / 4);
			const uint32 yBlocks = (paddedHeight / 4);

			uint8 pixels[16 * 4 * 4]{};
			std::array<Byte, 8> block{};

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

					rgbcx::encode_bc4_hq(block.data(), pixels);
					writer.write(block);
				}
			}
		}

		static void EncodeBC5(IWriter& writer, const Image& image)
		{
			const uint32 width = image.width();
			const uint32 height = image.height();

			const uint32 paddedWidth = ((width + 3) & ~3);
			const uint32 paddedHeight = ((height + 3) & ~3);

			const uint32 xBlocks = (paddedWidth / 4);
			const uint32 yBlocks = (paddedHeight / 4);

			uint8 pixels[16 * 4 * 4]{};
			std::array<Byte, 16> block{};

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

					rgbcx::encode_bc5_hq(block.data(), pixels);
					writer.write(block);
				}
			}
		}

		static void EncodeBC7(IWriter& writer, const Image& image)
		{
			const uint32 width = image.width();
			const uint32 height = image.height();

			const uint32 paddedWidth = ((width + 3) & ~3);
			const uint32 paddedHeight = ((height + 3) & ~3);

			const uint32 xBlocks = (paddedWidth / 4);
			const uint32 yBlocks = (paddedHeight / 4);

			bc7enc_compress_block_params bc7Params{};
			bc7enc_compress_block_params_init(&bc7Params);

			uint8 pixels[16 * 4 * 4]{};
			std::array<Byte, 16> block{};

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

					bc7enc_compress_block(block.data(), pixels, &bc7Params);
					writer.write(block);
				}
			}
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

	const Array<String>& BCnEncoder::possibleExtensions() const
	{
		static const Array<String> extensions{ U"dds" };

		return extensions;
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::save(const Image& image, const FilePathView path) const
	{
		BinaryWriter writer{ path };

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
		if (not writer.isOpen())
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

		WriteDDSHeader(writer, PIXEL_FMT_DXT1, width, height, 4, mipCount);

		Image tmpImage, previousImage;

		for (uint32 mip = 0; mip < mipCount; ++mip)
		{
			if (mip == 0)
			{
				EncodeBC1(writer, image);
			}
			else
			{
				const uint32 mipWidth = Max(1u, (width >> mip));
				const uint32 mipHeight = Max(1u, (height >> mip));

				previousImage.swap(tmpImage);

				if (not previousImage)
				{
					tmpImage = ImageProcessing::Resize(image, Size{ mipWidth, mipHeight });
				}
				else
				{
					tmpImage = ImageProcessing::Resize(previousImage, Size{ mipWidth, mipHeight });
				}
				
				EncodeBC1(writer, tmpImage);
			}
		}

		return true;
	}

	Blob BCnEncoder::encodeBC1(const Image& image, const GenerateMipmaps generateMipmaps) const
	{
		MemoryWriter writer;
	
		if (not encodeBC1(image, writer, generateMipmaps))
		{
			return{};
		}
		
		return writer.retrieve();
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeBC3
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::encodeBC3(const Image& image, IWriter& writer, const GenerateMipmaps generateMipmaps) const
	{
		if (not writer.isOpen())
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

		WriteDDSHeader(writer, PIXEL_FMT_DXT5, width, height, 8, mipCount);

		Image tmpImage, previousImage;

		for (uint32 mip = 0; mip < mipCount; ++mip)
		{
			if (mip == 0)
			{
				EncodeBC3(writer, image);
			}
			else
			{
				const uint32 mipWidth = Max(1u, (width >> mip));
				const uint32 mipHeight = Max(1u, (height >> mip));

				previousImage.swap(tmpImage);

				if (not previousImage)
				{
					tmpImage = ImageProcessing::Resize(image, Size{ mipWidth, mipHeight });
				}
				else
				{
					tmpImage = ImageProcessing::Resize(previousImage, Size{ mipWidth, mipHeight });
				}

				EncodeBC3(writer, tmpImage);
			}
		}

		return true;
	}

	Blob BCnEncoder::encodeBC3(const Image& image, const GenerateMipmaps generateMipmaps) const
	{
		MemoryWriter writer;

		if (not encodeBC3(image, writer, generateMipmaps))
		{
			return{};
		}

		return writer.retrieve();
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeBC4
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::encodeBC4(const Image& image, IWriter& writer, const GenerateMipmaps generateMipmaps) const
	{
		if (not writer.isOpen())
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

		WriteDDSHeader(writer, pixel_format(PIXEL_FMT_FOURCC('B', 'C', '4', 'U')), width, height, 4, mipCount);

		Image tmpImage, previousImage;

		for (uint32 mip = 0; mip < mipCount; ++mip)
		{
			if (mip == 0)
			{
				EncodeBC4(writer, image);
			}
			else
			{
				const uint32 mipWidth = Max(1u, (width >> mip));
				const uint32 mipHeight = Max(1u, (height >> mip));

				previousImage.swap(tmpImage);

				if (not previousImage)
				{
					tmpImage = ImageProcessing::Resize(image, Size{ mipWidth, mipHeight });
				}
				else
				{
					tmpImage = ImageProcessing::Resize(previousImage, Size{ mipWidth, mipHeight });
				}

				EncodeBC4(writer, tmpImage);
			}
		}

		return true;
	}

	Blob BCnEncoder::encodeBC4(const Image& image, const GenerateMipmaps generateMipmaps) const
	{
		MemoryWriter writer;

		if (not encodeBC4(image, writer, generateMipmaps))
		{
			return{};
		}

		return writer.retrieve();
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeBC5
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::encodeBC5(const Image& image, IWriter& writer, const GenerateMipmaps generateMipmaps) const
	{
		if (not writer.isOpen())
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

		WriteDDSHeader(writer, pixel_format(PIXEL_FMT_FOURCC('B', 'C', '5', 'U')), width, height, 8, mipCount);

		Image tmpImage, previousImage;

		for (uint32 mip = 0; mip < mipCount; ++mip)
		{
			if (mip == 0)
			{
				EncodeBC5(writer, image);
			}
			else
			{
				const uint32 mipWidth = Max(1u, (width >> mip));
				const uint32 mipHeight = Max(1u, (height >> mip));

				previousImage.swap(tmpImage);

				if (not previousImage)
				{
					tmpImage = ImageProcessing::Resize(image, Size{ mipWidth, mipHeight });
				}
				else
				{
					tmpImage = ImageProcessing::Resize(previousImage, Size{ mipWidth, mipHeight });
				}

				EncodeBC5(writer, tmpImage);
			}
		}

		return true;
	}

	Blob BCnEncoder::encodeBC5(const Image& image, const GenerateMipmaps generateMipmaps) const
	{
		MemoryWriter writer;
		
		if (not encodeBC5(image, writer, generateMipmaps))
		{
			return{};
		}
		
		return writer.retrieve();
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeBC7
	//
	////////////////////////////////////////////////////////////////

	bool BCnEncoder::encodeBC7(const Image& image, IWriter& writer, const GenerateMipmaps generateMipmaps) const
	{
		if (not writer.isOpen())
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

		WriteDDSHeader(writer, pixel_format(PIXEL_FMT_FOURCC('D', 'X', '1', '0')), width, height, 8, mipCount);
		WriteDX10Header(writer, DXGI_FORMAT_BC7_UNORM);

		Image tmpImage, previousImage;

		for (uint32 mip = 0; mip < mipCount; ++mip)
		{
			if (mip == 0)
			{
				EncodeBC7(writer, image);
			}
			else
			{
				const uint32 mipWidth = Max(1u, (width >> mip));
				const uint32 mipHeight = Max(1u, (height >> mip));

				previousImage.swap(tmpImage);

				if (not previousImage)
				{
					tmpImage = ImageProcessing::Resize(image, Size{ mipWidth, mipHeight });
				}
				else
				{
					tmpImage = ImageProcessing::Resize(previousImage, Size{ mipWidth, mipHeight });
				}

				EncodeBC7(writer, tmpImage);
			}
		}

		return true;
	}

	Blob BCnEncoder::encodeBC7(const Image& image, const GenerateMipmaps generateMipmaps) const
	{
		MemoryWriter writer;

		if (not encodeBC7(image, writer, generateMipmaps))
		{
			return{};
		}

		return writer.retrieve();
	}
}

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

# include "TextureUtility.hpp"
# include <Siv3D/BCnData.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/HalfFloat.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/ImageProcessing.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# if SIV3D_COMPILER(MSVC)
# include <atomic>
# endif

namespace s3d
{
	namespace
	{
		constexpr size_t ColorBlockBytes = 64;

		template <class Pixel>
		void FillColorRow(Byte* dst, size_t bytes, const Pixel* const block)
		{
			if constexpr (std::is_same_v<Pixel, uint8>)
			{
				std::memset(dst, *block, bytes);
			}
			else
			{
				while (ColorBlockBytes <= bytes)
				{
					std::memcpy(dst, block, ColorBlockBytes);
# if SIV3D_COMPILER(MSVC)
					// MSVC can turn repeated block stores into rep movs from the write-only destination.
					std::atomic_signal_fence(std::memory_order_acq_rel);
# endif
					dst += ColorBlockBytes;
					bytes -= ColorBlockBytes;
				}
				if (bytes)
				{
					std::memcpy(dst, block, bytes);
				}
			}
		}
	}

	std::span<const Blob> GetValidBCnMipmaps(const BCnData& data)
	{
		const Size size = data.size;
		if ((not data.format.isBCn()) || not InRange(size.x, 1, Image::MaxWidth) || not InRange(size.y, 1, Image::MaxHeight)
			|| ((size.x % 4) != 0) || ((size.y % 4) != 0))
		{
			return{};
		}

		const std::span<const Blob> mipmaps = data.textures;
		if (mipmaps.empty() || (mipmaps.front().size() != data.format.packedDataSize(size)))
		{
			return{};
		}

		for (uint32 level = 1; level < mipmaps.size(); ++level)
		{
			const Size levelSize = ImageProcessing::CalculateMipmapSize(size, level);
			if (levelSize.isZero())
			{
				LOG_WARN(fmt::format("BCn texture mip level {} follows the 1x1 level. Keeping {} level(s).", level, level));
				return mipmaps.first(level);
			}

			const size_t expected = data.format.packedDataSize(levelSize);
			const size_t actual = mipmaps[level].size();
			if (actual != expected)
			{
				LOG_WARN(fmt::format("BCn texture mip level {} expected {} bytes, got {}. Keeping {} level(s).", level, expected, actual, level));
				return mipmaps.first(level);
			}
		}
		return mipmaps;
	}

	bool IsValidRenderTexture(const Size& size, const std::span<const Byte> data, const TextureFormat& format) noexcept
	{
		// D3D11 / macOS Metal の共通上限。列挙値を検証してから format の表を参照する。
		if ((size.x <= 0) || (size.y <= 0) || (16384 < size.x) || (16384 < size.y)
			|| not InRange(format.value(), TexturePixelFormat::R8_Unorm, TexturePixelFormat::R32G32B32A32_Float))
		{
			return false;
		}
		const size_t dataSize = format.packedDataSize(size);
		return (data.empty() || (data.size_bytes() == dataSize));
	}

	bool ReadRenderTexture(const Texture& texture, const std::span<Byte> destination, const uint32 mipLevel)
	{
		const Size levelSize = texture.mipSize(mipLevel);
		if ((levelSize.x == 0) || (levelSize.y == 0))
		{
			return false;
		}
		if (destination.size_bytes() != texture.format().packedDataSize(levelSize))
		{
			return false;
		}
		return SIV3D_ENGINE(Texture)->readRT(texture.id(), destination, mipLevel);
	}

	Array<Byte> GenerateInitialColorBuffer(const Size& size, const ColorF& color, const TextureFormat& format)
	{
		if ((size.x <= 0) || (size.y <= 0))
		{
			return{};
		}

		const uint32 rowPitch = format.bytesPerRow(size.x);
		Array<Byte> bytes(static_cast<size_t>(rowPitch) * size.y);
		FillWithColor(bytes.data(), size, rowPitch, color, format);

		return bytes;
	}

	void FillWithColor(void* const pDst, const Size& size, const uint32 dstBytesPerRow, const ColorF& color, const TextureFormat& format)
	{
		const auto fill = [&]<class Pixel>(const Pixel pixel)
		{
			// Build the repeated pattern in CPU memory; upload memory is write-only.
			static_assert((ColorBlockBytes % sizeof(Pixel)) == 0);
			std::array<Pixel, ColorBlockBytes / sizeof(Pixel)> block;
			block.fill(pixel);

			auto* const dst = static_cast<Byte*>(pDst);
			const size_t rowBytes = (static_cast<size_t>(size.x) * sizeof(Pixel));
			if (rowBytes == dstBytesPerRow)
			{
				FillColorRow(dst, (rowBytes * size.y), block.data());
				return;
			}
			for (int32 y = 0; y < size.y; ++y)
			{
				FillColorRow(dst + static_cast<size_t>(y) * dstBytesPerRow, rowBytes, block.data());
			}
		};

		switch (format.value())
		{
			case TexturePixelFormat::R8_Unorm:
			{
				fill(color.toR8_Unorm());
				break;
			}
			case TexturePixelFormat::R8G8_Unorm:
			{
				fill(color.toR8G8_Unorm());
				break;
			}
			case TexturePixelFormat::R16_Float:
			{
				fill(color.toR16_Float().getBits());
				break;
			}
			case TexturePixelFormat::R8G8B8A8_Unorm:
			case TexturePixelFormat::R8G8B8A8_Unorm_SRGB:
			{
				fill(color.toR8G8B8A8_Unorm().asUint32());
				break;
			}
			case TexturePixelFormat::R16G16_Unorm:
			{
				fill(color.toR16G16_Unorm());
				break;
			}
			case TexturePixelFormat::R16G16_Float:
			{
				fill(color.toR16G16_Float());
				break;
			}
			case TexturePixelFormat::R32_Float:
			{
				fill(color.toR32_Float());
				break;
			}
			case TexturePixelFormat::R10G10B10A2_Unorm:
			{
				fill(color.toR10G10B10A2_Unorm());
				break;
			}
			case TexturePixelFormat::R11G11B10_UFloat:
			{
				fill(color.toR11G11B10_UFloat());
				break;
			}
			case TexturePixelFormat::R16G16B16A16_Float:
			{
				fill(color.toR16G16B16A16_Float());
				break;
			}
			case TexturePixelFormat::R32G32_Float:
			{
				fill(color.toR32G32_Float());
				break;
			}
			case TexturePixelFormat::R32G32B32A32_Float:
			{
				fill(color.toR32G32B32A32_Float());
				break;
			}
			case TexturePixelFormat::Unknown:
			case TexturePixelFormat::BC1_RGBA_Unorm:
			case TexturePixelFormat::BC1_RGBA_Unorm_SRGB:
			case TexturePixelFormat::BC3_RGBA_Unorm:
			case TexturePixelFormat::BC3_RGBA_Unorm_SRGB:
			case TexturePixelFormat::BC4_R_Unorm:
			case TexturePixelFormat::BC5_RG_Unorm:
			case TexturePixelFormat::BC7_RGBA_Unorm:
			case TexturePixelFormat::BC7_RGBA_Unorm_SRGB:
				// 単色のピクセル書き込みに対応しない形式
				break;
		}
	}

	size_t GetTextureUploadRowBytes(const Size size, const TextureFormat& format, const std::span<const Byte> src, const uint32 srcBytesPerRow) noexcept
	{
		if ((size.x <= 0) || (size.y <= 0) || format.isBCn())
		{
			return 0;
		}
		const size_t rowBytes = format.packedRowBytes(static_cast<uint32>(size.x));
		if ((rowBytes == 0) || (srcBytesPerRow < rowBytes)
			|| (src.size_bytes() < (uint64{ srcBytesPerRow } * (size.y - 1) + rowBytes)))
		{
			return 0;
		}
		return rowBytes;
	}

	void FillWithImage(void* const pDst, const Size& size, const uint32 dstBytesPerRow, const std::span<const Byte> src,
		const uint32 srcBytesPerRow, const size_t copyBytesPerRow)
	{
		if ((dstBytesPerRow == copyBytesPerRow) && (srcBytesPerRow == copyBytesPerRow))
		{
			std::memcpy(pDst, src.data(), (copyBytesPerRow * size.y));
		}
		else
		{
			auto* destination = static_cast<Byte*>(pDst);
			for (int32 y = 0; y < size.y; ++y)
			{
				std::memcpy(destination + static_cast<size_t>(y) * dstBytesPerRow,
					src.data() + static_cast<size_t>(y) * srcBytesPerRow, copyBytesPerRow);
			}
		}
	}
}

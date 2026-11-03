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
# include <Siv3D/HalfFloat.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		void Fill(void* pBytes, const size_t bufferSizeBytes, const uint8 value)
		{
			uint8* const pDst = static_cast<uint8*>(pBytes);
			uint8* const pEnd = (pDst + bufferSizeBytes);
			std::fill(pDst, pEnd, value);
		}

		void Fill(void* pBytes, const size_t bufferSizeBytes, const uint16 value)
		{
			const size_t count = (bufferSizeBytes / sizeof(uint16));
			uint16* const pDst = static_cast<uint16*>(pBytes);
			uint16* const pEnd = (pDst + count);
			std::fill(pDst, pEnd, value);
		}

		void Fill(void* pBytes, const size_t bufferSizeBytes, const uint32 value)
		{
			const size_t count = (bufferSizeBytes / sizeof(uint32));
			uint32* const pDst = static_cast<uint32*>(pBytes);
			uint32* const pEnd = (pDst + count);
			std::fill(pDst, pEnd, value);
		}

		void Fill(void* pBytes, const size_t bufferSizeBytes, const float value)
		{
			const size_t count = (bufferSizeBytes / sizeof(float));
			float* const pDst = static_cast<float*>(pBytes);
			float* const pEnd = (pDst + count);
			std::fill(pDst, pEnd, value);
		}

		void Fill(void* pBytes, const size_t bufferSizeBytes, const uint64 value)
		{
			const size_t count = (bufferSizeBytes / sizeof(uint64));
			uint64* const pDst = static_cast<uint64*>(pBytes);
			uint64* const pEnd = (pDst + count);
			std::fill(pDst, pEnd, value);
		}

		void Fill(void* pBytes, const size_t bufferSizeBytes, const Float2 value)
		{
			const size_t count = (bufferSizeBytes / sizeof(Float2));
			Float2* const pDst = static_cast<Float2*>(pBytes);
			Float2* const pEnd = (pDst + count);
			std::fill(pDst, pEnd, value);
		}

		void Fill(void* pBytes, const size_t bufferSizeBytes, const Float4 value)
		{
			const size_t count = (bufferSizeBytes / sizeof(Float4));
			Float4* const pDst = static_cast<Float4*>(pBytes);
			Float4* const pEnd = (pDst + count);
			std::fill(pDst, pEnd, value);
		}
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

		// 1 行のバイト数
		const size_t bytesPerRow = format.bytesPerRow(size.x);

		// 全体のバイト数
		const size_t bufferSizeBytes = (bytesPerRow * size.y);

		Array<Byte> bytes(bufferSizeBytes);
		
		void* pBytes = static_cast<void*>(bytes.data());

		switch (format.value())
		{
		case TexturePixelFormat::R8_Unorm:
			{
				Fill(pBytes, bufferSizeBytes, color.toR8_Unorm());
				break;
			}
		case TexturePixelFormat::R8G8_Unorm:
			{
				Fill(pBytes, bufferSizeBytes, color.toR8G8_Unorm());
				break;
			}
		case TexturePixelFormat::R16_Float:
			{
				Fill(pBytes, bufferSizeBytes, color.toR16_Float().getBits());
				break;
			}
		case TexturePixelFormat::R8G8B8A8_Unorm:
		case TexturePixelFormat::R8G8B8A8_Unorm_SRGB:
			{
				Fill(pBytes, bufferSizeBytes, color.toR8G8B8A8_Unorm().asUint32());
				break;
			}
		case TexturePixelFormat::R16G16_Unorm:
			{
				Fill(pBytes, bufferSizeBytes, color.toR16G16_Unorm());
				break;
			}
		case TexturePixelFormat::R16G16_Float:
			{
				Fill(pBytes, bufferSizeBytes, color.toR16G16_Float());
				break;
			}
		case TexturePixelFormat::R32_Float:
			{
				Fill(pBytes, bufferSizeBytes, color.toR32_Float());
				break;
			}
		case TexturePixelFormat::R10G10B10A2_Unorm:
			{
				Fill(pBytes, bufferSizeBytes, color.toR10G10B10A2_Unorm());
				break;
			}
		case TexturePixelFormat::R11G11B10_UFloat:
			{
				Fill(pBytes, bufferSizeBytes, color.toR11G11B10_UFloat());
				break;
			}
		case TexturePixelFormat::R16G16B16A16_Float:
			{
				Fill(pBytes, bufferSizeBytes, color.toR16G16B16A16_Float());
				break;
			}
		case TexturePixelFormat::R32G32_Float:
			{
				Fill(pBytes, bufferSizeBytes, color.toR32G32_Float());
				break;
			}
		case TexturePixelFormat::R32G32B32A32_Float:
			{
				Fill(pBytes, bufferSizeBytes, color.toR32G32B32A32_Float());
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

		return bytes;
	}

	void FillWithColor(void* const pDst, const size_t bufferSizeBytes, const ColorF& color, const TextureFormat& format)
	{
		switch (format.value())
		{
			case TexturePixelFormat::R8_Unorm:
			{
				Fill(pDst, bufferSizeBytes, color.toR8_Unorm());
				break;
			}
			case TexturePixelFormat::R8G8_Unorm:
			{
				Fill(pDst, bufferSizeBytes, color.toR8G8_Unorm());
				break;
			}
			case TexturePixelFormat::R16_Float:
			{
				Fill(pDst, bufferSizeBytes, color.toR16_Float().getBits());
				break;
			}
			case TexturePixelFormat::R8G8B8A8_Unorm:
			case TexturePixelFormat::R8G8B8A8_Unorm_SRGB:
			{
				Fill(pDst, bufferSizeBytes, color.toR8G8B8A8_Unorm().asUint32());
				break;
			}
			case TexturePixelFormat::R16G16_Unorm:
			{
				Fill(pDst, bufferSizeBytes, color.toR16G16_Unorm());
				break;
			}
			case TexturePixelFormat::R16G16_Float:
			{
				Fill(pDst, bufferSizeBytes, color.toR16G16_Float());
				break;
			}
			case TexturePixelFormat::R32_Float:
			{
				Fill(pDst, bufferSizeBytes, color.toR32_Float());
				break;
			}
			case TexturePixelFormat::R10G10B10A2_Unorm:
			{
				Fill(pDst, bufferSizeBytes, color.toR10G10B10A2_Unorm());
				break;
			}
			case TexturePixelFormat::R11G11B10_UFloat:
			{
				Fill(pDst, bufferSizeBytes, color.toR11G11B10_UFloat());
				break;
			}
			case TexturePixelFormat::R16G16B16A16_Float:
			{
				Fill(pDst, bufferSizeBytes, color.toR16G16B16A16_Float());
				break;
			}
			case TexturePixelFormat::R32G32_Float:
			{
				Fill(pDst, bufferSizeBytes, color.toR32G32_Float());
				break;
			}
			case TexturePixelFormat::R32G32B32A32_Float:
			{
				Fill(pDst, bufferSizeBytes, color.toR32G32B32A32_Float());
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

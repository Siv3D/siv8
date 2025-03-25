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

# include "TextureUtility.hpp"
# include <Siv3D/HalfFloat.hpp>

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
		}

		return bytes;
	}
}

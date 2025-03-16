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

# pragma once
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

		[[nodiscard]]
		uint32 ToR10G10B10A2(const ColorF color)
		{
			const double r = Clamp(color.r, 0.0, 1.0);
			const double g = Clamp(color.g, 0.0, 1.0);
			const double b = Clamp(color.b, 0.0, 1.0);
			const double a = Clamp(color.a, 0.0, 1.0);

			const uint32 rBits = static_cast<uint32>(r * 1023.0 + 0.5); // 10 ビット (0-1023)
			const uint32 gBits = static_cast<uint32>(g * 1023.0 + 0.5); // 10 ビット (0-1023)
			const uint32 bBits = static_cast<uint32>(b * 1023.0 + 0.5); // 10 ビット (0-1023)
			const uint32 aBits = static_cast<uint32>(a * 3.0 + 0.5);    //  2 ビット (0-3)

			return ((rBits) | (gBits << 10) | (bBits << 20) | (aBits << 30));
		}

		[[nodiscard]]
		uint32 ToR11G11B10_UFloat(const ColorF color)
		{
			// 負の値は 0 にクランプ（unsigned float形式のため）
			const float r = Max(static_cast<float>(color.r), 0.0f);
			const float g = Max(static_cast<float>(color.g), 0.0f);
			const float b = Max(static_cast<float>(color.b), 0.0f);

			// R と G の 11-bit 浮動小数点変換 (5 ビット指数, 6 ビット仮数)
			uint32 rBits = 0;
			if (0.0f < r)
			{
				int32 rExp;
				const float rMantissa = std::frexpf(r, &rExp);
				rExp += 14; // バイアス調整 (15-1)

				if (rExp <= 0)
				{
					// デノーマル数（非常に小さい値）またはゼロ
					rBits = 0;
				}
				else if (31 <= rExp)
				{
					// オーバーフロー - 最大値に設定
					rBits = 0x7FF;
				}
				else
				{
					const uint32 rMantissaBits = static_cast<uint32>(rMantissa * 64.0f);
					rBits = ((rExp << 6) | (rMantissaBits & 0x3F));
				}
			}

			// G の 11-bit 浮動小数点変換
			uint32 gBits = 0;
			if (0.0f < g)
			{
				int32 gExp;
				const float gMantissa = std::frexpf(g, &gExp);
				gExp += 14; // バイアス調整

				if (gExp <= 0)
				{
					gBits = 0;
				}
				else if (31 <= gExp)
				{
					gBits = 0x7FF;
				}
				else
				{
					const uint32 gMantissaBits = static_cast<uint32>(gMantissa * 64.0f);
					gBits = ((gExp << 6) | (gMantissaBits & 0x3F));
				}
			}

			// B の 10-bit 浮動小数点変換 (5 ビット指数, 5 ビット仮数)
			uint32 bBits = 0;
			if (0.0f < b)
			{
				int32 bExp;
				const float bMantissa = std::frexpf(b, &bExp);
				bExp += 14; // バイアス調整

				if (bExp <= 0)
				{
					bBits = 0;
				}
				else if (31 <= bExp)
				{
					bBits = 0x3FF;
				}
				else
				{
					const uint32 bMantissaBits = static_cast<uint32>(bMantissa * 32.0f);
					bBits = ((bExp << 5) | (bMantissaBits & 0x1F));
				}
			}

			// R11G11B10 形式にパック
			return (rBits | (gBits << 11) | (bBits << 22));
		}
	}

	Array<Byte> GenerateInitialColorBuffer(const Size& size, const ColorF& color, const TextureFormat& format)
	{
		if ((size.x <= 0) || (size.y <= 0))
		{
			return{};
		}

		// 1 行のバイト数
		const size_t rowStride = (((size.x * format.pixelSize()) + 3) / 4 * 4);

		// 全体のバイト数
		const size_t bufferSizeBytes = (rowStride * size.y);

		const Color color8 = color.toColor();

		Array<Byte> bytes(bufferSizeBytes);
		
		void* pBytes = static_cast<void*>(bytes.data());

		switch (format.value())
		{
		case TexturePixelFormat::R8_Unorm:
			{
				Fill(pBytes, bufferSizeBytes, color8.r);
				break;
			}
		case TexturePixelFormat::R8G8_Unorm:
			{
				const uint16 rg = ((static_cast<uint16>(color8.g) << 8) | color8.r);
				Fill(pBytes, bufferSizeBytes, rg);
				break;
			}
		case TexturePixelFormat::R16_Float:
			{
				const uint16 r = HalfFloat{ color.r }.getBits();
				Fill(pBytes, bufferSizeBytes, r);
				break;
			}
		case TexturePixelFormat::R8G8B8A8_Unorm:
		case TexturePixelFormat::R8G8B8A8_Unorm_SRGB:
			{
				const uint32 rgba = color8.asUint32();
				Fill(pBytes, bufferSizeBytes, rgba);
				break;
			}
		case TexturePixelFormat::R16G16_Unorm:
			{
				const uint16 r = static_cast<uint16>(Clamp(color.r, 0.0, 1.0) * 65535.0 + 0.5);
				const uint16 g = static_cast<uint16>(Clamp(color.g, 0.0, 1.0) * 65535.0 + 0.5);
				const uint32 rg = ((g << 16) | r);
				Fill(pBytes, bufferSizeBytes, rg);
				break;
			}
		case TexturePixelFormat::R16G16_Float:
			{
				const uint32 rg = ((static_cast<uint32>(HalfFloat{ color.g }.getBits()) << 16) | HalfFloat{ color.r }.getBits());
				Fill(pBytes, bufferSizeBytes, rg);
				break;
			}
		case TexturePixelFormat::R32_Float:
			{
				Fill(pBytes, bufferSizeBytes, static_cast<float>(color.r));
				break;
			}
		case TexturePixelFormat::R10G10B10A2_Unorm:
			{
				const uint32 rgba = ToR10G10B10A2(color);
				Fill(pBytes, bufferSizeBytes, rgba);
				break;
			}
		case TexturePixelFormat::R11G11B10_UFloat:
			{
				const uint32 rgb = ToR11G11B10_UFloat(color);
				Fill(pBytes, bufferSizeBytes, rgb);
				break;
			}
		case TexturePixelFormat::R16G16B16A16_Float:
			{
				const uint16 r = HalfFloat{ color.r }.getBits();
				const uint16 g = HalfFloat{ color.g }.getBits();
				const uint16 b = HalfFloat{ color.b }.getBits();
				const uint16 a = HalfFloat{ color.a }.getBits();
				const uint64 rgba = ((static_cast<uint64>(a) << 48) | (static_cast<uint64>(b) << 32) | (static_cast<uint64>(g) << 16) | r);
				Fill(pBytes, bufferSizeBytes, rgba);
				break;
			}
		case TexturePixelFormat::R32G32_Float:
			{
				Fill(pBytes, bufferSizeBytes, color.rg());
				break;
			}
		case TexturePixelFormat::R32G32B32A32_Float:
			{
				Fill(pBytes, bufferSizeBytes, color.rgba());
				break;
			}
		}

		return bytes;
	}
}

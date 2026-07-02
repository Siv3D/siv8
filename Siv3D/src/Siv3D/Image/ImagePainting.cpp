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

# include <Siv3D/SIMD.hpp>
# include "ImagePainting.hpp"

namespace s3d
{
	static_assert(sizeof(Color) == 4);
	static_assert(std::is_trivially_copyable_v<Color>);

	namespace ImagePainting
	{
	# if SIV3D_INTRINSIC(SSE)

		namespace simd
		{
			static const __m128 c255 = ::_mm_set_ps1(255.0f);

			static const __m128i c255i = ::_mm_set1_epi32(255);

			static const __m128 inv255 = ::_mm_set_ps1(1.0f / 255.0f);

			static const __m128 inv255pow2 = ::_mm_set_ps1(1.0f / (255.0f * 255.0f));

			static const __m128 inv255pow3 = ::_mm_set_ps1(1.0f / (255.0f * 255.0f * 255.0f));

			static const __m128i toColorShuffleMask = ::_mm_setr_epi8(0, 4, 8, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
		}

		void Paint_SSE4_1(const Color* pSrc, Color* pDst,
			const int32 width, const int32 height, const int32 srcWidth, const int32 dstWidth, const Color color)
		{
			const int32 srcStepOffset = srcWidth - width;
			const int32 dstStepOffset = dstWidth - width;
			const uint32 globalSrcAlpha = color.a;

			if (color == Palette::White)
			{
				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint8 t = pDst->a;
						const uint32 srcAlpha = pSrc->a;
						const uint32 dstAlpha = 255 - srcAlpha;

						const __m128i dstA = ::_mm_set1_epi32(dstAlpha);
						const __m128i srcA = ::_mm_set1_epi32(srcAlpha);

						const __m128i dsti = ::_mm_cvtepu8_epi32(_mm_loadu_si32(pDst)); // SSE4.1
						const __m128i srci = ::_mm_cvtepu8_epi32(_mm_loadu_si32(pSrc)); // SSE4.1

						const __m128i left = ::_mm_mullo_epi32(dsti, dstA); // SSE4.1
						const __m128i right = ::_mm_mullo_epi32(srci, srcA); // SSE4.1
						const __m128i sumi = ::_mm_add_epi32(left, right);
						const __m128 sum = ::_mm_cvtepi32_ps(sumi);

						const __m128 result = ::_mm_mul_ps(sum, simd::inv255);
						const __m128i r = ::_mm_cvtps_epi32(result);

						_mm_storeu_si32(pDst, ::_mm_shuffle_epi8(r, simd::toColorShuffleMask));
						pDst->a = t;

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
			else if (color.r == 255 && color.g == 255 && color.b == 255)
			{
				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint8 t = pDst->a;
						const uint32 srcAlpha = pSrc->a * globalSrcAlpha;
						const uint32 dstAlpha = (255 * 255) - srcAlpha;

						const __m128i dstA = ::_mm_set1_epi32(dstAlpha);
						const __m128i srcA = ::_mm_set1_epi32(srcAlpha);

						const __m128i dsti = ::_mm_cvtepu8_epi32(_mm_loadu_si32(pDst)); // SSE4.1
						const __m128i srci = ::_mm_cvtepu8_epi32(_mm_loadu_si32(pSrc)); // SSE4.1

						const __m128i left = ::_mm_mullo_epi32(dsti, dstA); // SSE4.1
						const __m128i right = ::_mm_mullo_epi32(srci, srcA); // SSE4.1
						const __m128i sumi = ::_mm_add_epi32(left, right);
						const __m128 sum = ::_mm_cvtepi32_ps(sumi);

						const __m128 result = ::_mm_mul_ps(sum, simd::inv255pow2);
						const __m128i r = ::_mm_cvtps_epi32(result);

						_mm_storeu_si32(pDst, ::_mm_shuffle_epi8(r, simd::toColorShuffleMask));
						pDst->a = t;

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
			else if (color.a == 255)
			{
				const __m128i gi = ::_mm_cvtepu8_epi32(::_mm_loadu_si32(&color)); // SSE4.1

				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint8 t = pDst->a;
						const uint32 srcAlpha = pSrc->a;
						const uint32 dstAlpha = 255 - srcAlpha;

						const __m128i dstA = ::_mm_set1_epi32(dstAlpha);
						const __m128i srcA = ::_mm_set1_epi32(srcAlpha);

						const __m128i dsti = ::_mm_cvtepu8_epi32(_mm_loadu_si32(pDst)); // SSE4.1
						const __m128i srci = ::_mm_cvtepu8_epi32(_mm_loadu_si32(pSrc)); // SSE4.1

						const __m128i left = ::_mm_mullo_epi32(::_mm_mullo_epi32(simd::c255i, dsti), dstA); // SSE4.1
						const __m128i right = ::_mm_mullo_epi32(::_mm_mullo_epi32(gi, srci), srcA); // SSE4.1
						const __m128i sumi = ::_mm_add_epi32(left, right);
						const __m128 sum = ::_mm_cvtepi32_ps(sumi);

						const __m128 result = ::_mm_mul_ps(sum, simd::inv255pow2);
						const __m128i r = ::_mm_cvtps_epi32(result);

						_mm_storeu_si32(pDst, ::_mm_shuffle_epi8(r, simd::toColorShuffleMask));
						pDst->a = t;

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
			else
			{
				const __m128i gi = ::_mm_cvtepu8_epi32(_mm_loadu_si32(&color)); // SSE4.1
				const __m128 g = ::_mm_cvtepi32_ps(gi);

				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint8 t = pDst->a;
						const uint32 srcAlpha = pSrc->a * globalSrcAlpha;
						const uint32 dstAlpha = (255 * 255) - srcAlpha;
						const __m128 dstA = ::_mm_set_ps1(static_cast<float>(dstAlpha));
						const __m128 srcA = ::_mm_set_ps1(static_cast<float>(srcAlpha));

						const __m128i dsti = ::_mm_cvtepu8_epi32(_mm_loadu_si32(pDst)); // SSE4.1
						const __m128 dst = ::_mm_cvtepi32_ps(dsti);

						const __m128i srci = ::_mm_cvtepu8_epi32(_mm_loadu_si32(pSrc)); // SSE4.1
						const __m128 src = ::_mm_cvtepi32_ps(srci);

						const __m128 left = ::_mm_mul_ps(::_mm_mul_ps(simd::c255, dst), dstA);
						const __m128 right = ::_mm_mul_ps(::_mm_mul_ps(g, src), srcA);
						const __m128 sum = ::_mm_add_ps(left, right);

						const __m128 result = ::_mm_mul_ps(sum, simd::inv255pow3);
						const __m128i r = ::_mm_cvtps_epi32(result);

						_mm_storeu_si32(pDst, ::_mm_shuffle_epi8(r, simd::toColorShuffleMask));
						pDst->a = t;

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
		}

	# endif
		
		void Paint_Reference(const Color* pSrc, Color* pDst,
			const int32 width, const int32 height, const int32 srcWidth, const int32 dstWidth, const Color color)
		{
			const int32 srcStepOffset = srcWidth - width;
			const int32 dstStepOffset = dstWidth - width;
			const uint32 globalSrcRed = color.r;
			const uint32 globalSrcGreen = color.g;
			const uint32 globalSrcBlue = color.b;
			const uint32 globalSrcAlpha = color.a;

			if (color == Palette::White)
			{
				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint32 srcAlpha = pSrc->a;

						if (srcAlpha != 255)
						{
							const uint32 dstAlpha = 255 - srcAlpha;
							pDst->r = static_cast<uint8>((pDst->r * dstAlpha + pSrc->r * srcAlpha) / 255);
							pDst->g = static_cast<uint8>((pDst->g * dstAlpha + pSrc->g * srcAlpha) / 255);
							pDst->b = static_cast<uint8>((pDst->b * dstAlpha + pSrc->b * srcAlpha) / 255);
						}
						else
						{
							const uint8 a = pDst->a;

							*pDst = *pSrc;

							pDst->a = a;
						}

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
			else if (color.r == 255 && color.g == 255 && color.b == 255)
			{
				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint32 srcAlpha = pSrc->a * globalSrcAlpha;
						const uint32 dstAlpha = (255 * 255) - srcAlpha;

						pDst->r = static_cast<uint8>((pDst->r * dstAlpha + pSrc->r * srcAlpha) / (255 * 255));
						pDst->g = static_cast<uint8>((pDst->g * dstAlpha + pSrc->g * srcAlpha) / (255 * 255));
						pDst->b = static_cast<uint8>((pDst->b * dstAlpha + pSrc->b * srcAlpha) / (255 * 255));

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
			else if (color.a == 255)
			{
				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint32 srcAlpha = pSrc->a;

						if (srcAlpha != 255)
						{
							const uint32 dstAlpha = 255 - srcAlpha;
							pDst->r = static_cast<uint8>((255 * pDst->r * dstAlpha + globalSrcRed * pSrc->r * srcAlpha) / (255 * 255));
							pDst->g = static_cast<uint8>((255 * pDst->g * dstAlpha + globalSrcGreen * pSrc->g * srcAlpha) / (255 * 255));
							pDst->b = static_cast<uint8>((255 * pDst->b * dstAlpha + globalSrcBlue * pSrc->b * srcAlpha) / (255 * 255));
						}
						else
						{
							pDst->r = static_cast<uint8>((globalSrcRed * pSrc->r) / 255);
							pDst->g = static_cast<uint8>((globalSrcGreen * pSrc->g) / 255);
							pDst->b = static_cast<uint8>((globalSrcBlue * pSrc->b) / 255);
						}

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
			else
			{
				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint32 srcAlpha = pSrc->a * globalSrcAlpha;
						const uint32 dstAlpha = (255 * 255) - srcAlpha;

						pDst->r = static_cast<uint8>((255 * pDst->r * dstAlpha + globalSrcRed * pSrc->r * srcAlpha) / (255 * 255 * 255));
						pDst->g = static_cast<uint8>((255 * pDst->g * dstAlpha + globalSrcGreen * pSrc->g * srcAlpha) / (255 * 255 * 255));
						pDst->b = static_cast<uint8>((255 * pDst->b * dstAlpha + globalSrcBlue * pSrc->b * srcAlpha) / (255 * 255 * 255));

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
		}

		void Paint(const Color* pSrc, Color* pDst,
			const int32 width, const int32 height, const int32 srcWidth, const int32 dstWidth, const Color color)
		{
		# if SIV3D_INTRINSIC(SSE)

			return Paint_SSE4_1(pSrc, pDst, width, height, srcWidth, dstWidth, color);

		# else
		
			// [Siv3D ToDo] Neon 実装
			Paint_Reference(pSrc, pDst, width, height, srcWidth, dstWidth, color);

		# endif
		}

		void Stamp_Reference(const Color* pSrc, Color* pDst,
			const int32 width, const int32 height, const int32 srcWidth, const int32 dstWidth, const Color color)
		{
			const int32 srcStepOffset = srcWidth - width;
			const int32 dstStepOffset = dstWidth - width;
			const uint32 globalSrcRed = color.r;
			const uint32 globalSrcGreen = color.g;
			const uint32 globalSrcBlue = color.b;
			const uint32 globalSrcAlpha = color.a;

			if (color == Palette::White)
			{
				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint32 srcAlpha = pSrc->a;
						const uint32 dstAlpha = pDst->a;

						if (srcAlpha == 255)
						{
							*pDst = *pSrc;
						}
						else if (dstAlpha == 0)
						{
							*pDst = *pSrc;
						}
						else
						{
							const uint32 dstBlend = (255 - srcAlpha);
							pDst->r = static_cast<uint8>((pDst->r * dstBlend + pSrc->r * srcAlpha) / 255);
							pDst->g = static_cast<uint8>((pDst->g * dstBlend + pSrc->g * srcAlpha) / 255);
							pDst->b = static_cast<uint8>((pDst->b * dstBlend + pSrc->b * srcAlpha) / 255);
							pDst->a = Max(pSrc->a, pDst->a);
						}

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
			else if (color.r == 255 && color.g == 255 && color.b == 255)
			{
				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint32 srcAlpha = (pSrc->a * globalSrcAlpha);
						const uint32 dstBlend = (255 * 255) - srcAlpha;
						const uint32 dstAlpha = pDst->a;

						if (dstAlpha == 0)
						{
							pDst->r = pSrc->r;
							pDst->g = pSrc->g;
							pDst->b = pSrc->b;
							pDst->a = static_cast<uint8>(srcAlpha / 255);
						}
						else
						{
							pDst->r = static_cast<uint8>((pDst->r * dstBlend + pSrc->r * srcAlpha) / (255 * 255));
							pDst->g = static_cast<uint8>((pDst->g * dstBlend + pSrc->g * srcAlpha) / (255 * 255));
							pDst->b = static_cast<uint8>((pDst->b * dstBlend + pSrc->b * srcAlpha) / (255 * 255));
							pDst->a = static_cast<uint8>(Max((srcAlpha / 255), dstAlpha));
						}

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
			else if (color.a == 255)
			{
				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint32 srcAlpha = pSrc->a;

						if (srcAlpha != 255)
						{
							const uint32 dstBlend = 255 - srcAlpha;
							pDst->r = static_cast<uint8>((255 * pDst->r * dstBlend + globalSrcRed * pSrc->r * srcAlpha) / (255 * 255));
							pDst->g = static_cast<uint8>((255 * pDst->g * dstBlend + globalSrcGreen * pSrc->g * srcAlpha) / (255 * 255));
							pDst->b = static_cast<uint8>((255 * pDst->b * dstBlend + globalSrcBlue * pSrc->b * srcAlpha) / (255 * 255));
							pDst->a = Max(pSrc->a, pDst->a);
						}
						else
						{
							pDst->r = static_cast<uint8>((globalSrcRed * pSrc->r) / 255);
							pDst->g = static_cast<uint8>((globalSrcGreen * pSrc->g) / 255);
							pDst->b = static_cast<uint8>((globalSrcBlue * pSrc->b) / 255);
							pDst->a = Max(pSrc->a, pDst->a);
						}

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
			else
			{
				for (int32 y = 0; y < height; ++y)
				{
					for (int32 x = 0; x < width; ++x)
					{
						const uint32 srcAlpha = pSrc->a * globalSrcAlpha;
						const uint32 dstBlend = (255 * 255) - srcAlpha;
						const uint32 dstAlpha = pDst->a;

						pDst->r = static_cast<uint8>((255 * pDst->r * dstBlend + globalSrcRed * pSrc->r * srcAlpha) / (255 * 255 * 255));
						pDst->g = static_cast<uint8>((255 * pDst->g * dstBlend + globalSrcGreen * pSrc->g * srcAlpha) / (255 * 255 * 255));
						pDst->b = static_cast<uint8>((255 * pDst->b * dstBlend + globalSrcBlue * pSrc->b * srcAlpha) / (255 * 255 * 255));
						pDst->a = static_cast<uint8>(Max((srcAlpha / 255), dstAlpha));

						++pSrc;
						++pDst;
					}

					pSrc += srcStepOffset;
					pDst += dstStepOffset;
				}
			}
		}

		void Stamp(const Color* pSrc, Color* pDst,
			const int32 width, const int32 height, const int32 srcWidth, const int32 dstWidth, const Color color)
		{
			Stamp_Reference(pSrc, pDst, width, height, srcWidth, dstWidth, color);
		}

		void Overwrite(const Color* pSrc, Color* pDst,
			const int32 width, const int32 height, const int32 srcWidth, const int32 dstWidth)
		{
			const size_t stride_bytes = (width * sizeof(Color));

			for (int32 y = 0; y < height; ++y)
			{
				std::memcpy(pDst, pSrc, stride_bytes);
				pSrc += srcWidth;
				pDst += dstWidth;
			}
		}
	}
}

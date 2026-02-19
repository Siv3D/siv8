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

# include <Siv3D/Image.hpp>
# include <Siv3D/CPUInfo.hpp>
# include <Siv3D/SIMD.hpp>

namespace s3d
{
	namespace
	{
		static constexpr uint32_t g_unmultiplyAlphaTable[256] = {
			0, 16711680, 8355840, 5570560, 4177920, 3342336, 2785280, 2387383, 2088960, 1856853, 1671168, 1519244, 1392640, 1285514, 1193691, 1114112,
			1044480, 983040, 928427, 879562, 835584, 795794, 759622, 726595, 696320, 668467, 642757, 618951, 596846, 576265, 557056, 539086,
			522240, 506415, 491520, 477477, 464213, 451667, 439781, 428505, 417792, 407602, 397897, 388644, 379811, 371371, 363297, 355568,
			348160, 341055, 334234, 327680, 321378, 315315, 309476, 303849, 298423, 293187, 288132, 283249, 278528, 273962, 269543, 265265,
			261120, 257103, 253207, 249428, 245760, 242198, 238738, 235376, 232107, 228927, 225834, 222822, 219891, 217035, 214252, 211540,
			208896, 206317, 203801, 201346, 198949, 196608, 194322, 192088, 189905, 187772, 185685, 183645, 181649, 179695, 177784, 175912,
			174080, 172285, 170527, 168805, 167117, 165462, 163840, 162249, 160689, 159159, 157657, 156184, 154738, 153318, 151924, 150556,
			149211, 147891, 146594, 145319, 144066, 142835, 141624, 140434, 139264, 138113, 136981, 135867, 134772, 133693, 132632, 131588,
			130560, 129548, 128551, 127570, 126604, 125652, 124714, 123790, 122880, 121983, 121099, 120228, 119369, 118523, 117688, 116865,
			116053, 115253, 114464, 113685, 112917, 112159, 111411, 110673, 109945, 109227, 108517, 107817, 107126, 106444, 105770, 105105,
			104448, 103799, 103159, 102526, 101900, 101283, 100673, 100070, 99474, 98886, 98304, 97729, 97161, 96599, 96044, 95495,
			94953, 94416, 93886, 93361, 92843, 92330, 91822, 91321, 90824, 90333, 89848, 89367, 88892, 88422, 87956, 87496,
			87040, 86589, 86143, 85701, 85264, 84831, 84402, 83978, 83558, 83143, 82731, 82324, 81920, 81520, 81125, 80733,
			80345, 79960, 79579, 79202, 78829, 78459, 78092, 77729, 77369, 77012, 76659, 76309, 75962, 75618, 75278, 74940,
			74606, 74274, 73945, 73620, 73297, 72977, 72659, 72345, 72033, 71724, 71417, 71114, 70812, 70513, 70217, 69923,
			69632, 69343, 69057, 68772, 68490, 68211, 67934, 67659, 67386, 67115, 66847, 66580, 66316, 66054, 65794, 65536
		};

		static void UnmultiplyAlpha_plain(Color* pixels, const size_t num_pixels)
		{
			Color* p = pixels;
			const Color* const pEnd = (pixels + num_pixels);

			while (p != pEnd)
			{
				const uint32 a = p->a;

				if (a == 0)
				{
					p->r = p->g = p->b = 0;
				}
				else if (a != 255)
				{
					const uint32 invQ16 = g_unmultiplyAlphaTable[a];

					auto Unpremul = [invQ16](uint8 c) -> uint8
					{
						const uint32 v = (static_cast<uint32>(c) * invQ16 + 0x8000u) >> 16;
						return static_cast<uint8>(v > 255u ? 255u : v);
					};

					p->r = Unpremul(p->r);
					p->g = Unpremul(p->g);
					p->b = Unpremul(p->b);
				}

				++p;
			}
		}

		static void PremultiplyAlpha_plain(Color* pixels, const size_t num_pixels)
		{
			Color* p = pixels;
			const Color* const pEnd = (pixels + num_pixels);

			while (p != pEnd)
			{
				const uint32 a = p->a;
				p->r = Color::Div255Round(static_cast<uint32>(p->r) * a);
				p->g = Color::Div255Round(static_cast<uint32>(p->g) * a);
				p->b = Color::Div255Round(static_cast<uint32>(p->b) * a);
				++p;
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	https://github.com/Wizermil/premultiply_alpha
		//
		//	Copyright (c) 2019 Mathieu Garaud
		//
		//	Permission is hereby granted, free of charge, to any person obtaining a copy
		//	of this software and associated documentation files (the "Software"), to deal
		//	in the Software without restriction, including without limitation the rights
		//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
		//	copies of the Software, and to permit persons to whom the Software is
		//	furnished to do so, subject to the following conditions:
		//
		//	The above copyright notice and this permission notice shall be included in all
		//	copies or substantial portions of the Software.
		//
		//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
		//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
		//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
		//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
		//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
		//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
		//	SOFTWARE.

	# if SIV3D_PLATFORM(MACOS)

		static void PremultiplyAlpha_GCC(Color* pixels, const size_t num_pixels)
		{
			using u8x16 = __attribute__((ext_vector_type(16))) uint8_t;
			using u16x8 = __attribute__((ext_vector_type(8))) uint16_t;

			size_t const loopCount = ((num_pixels + 3) / 4);

			for (u8x16* ptr = reinterpret_cast<u8x16*>(pixels), *end = (ptr + loopCount); ptr != end; ++ptr)
			{
				const auto p = *ptr;
				u16x8 const alpha{ p[3], p[3], p[7], p[7], p[11], p[11], p[15], p[15] };
				u16x8 color_even{ (reinterpret_cast<u16x8>(p) >> 8) | u16x8{ 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF } };
				u16x8 color_odd{ reinterpret_cast<u16x8>(p) & u16x8 { 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF } };
				color_even = color_even * alpha;
				color_odd = color_odd * alpha;
				color_even /= 255U;
				color_odd /= 255U;
				*ptr = reinterpret_cast<u8x16>((color_even << 8) | color_odd);
			}
		}

	# endif

	# if SIV3D_INTRINSIC(SSE)

		static void PremultiplyAlpha_SSE41(Color* pixels, const size_t num_pixels)
		{
			const size_t loopCount = ((num_pixels + 3) / (sizeof(__m128i) / sizeof(Color)) / 2);
			uint32* p = reinterpret_cast<uint32*>(pixels);

			for (__m128i* ptr = reinterpret_cast<__m128i*>(p), *end = (ptr + loopCount * 2); ptr != end; ptr += 2)
			{
				// load data and split channels:
				__m128i abgr = _mm_load_si128(ptr);
				__m128i ABGR = _mm_load_si128(ptr + 1);
				__m128i __ab = _mm_srli_epi32(abgr, 16);
				__m128i GR__ = _mm_slli_epi32(ABGR, 16);
				__m128i ABab = _mm_blend_epi16(ABGR, __ab, 0x55);
				__m128i GRgr = _mm_blend_epi16(GR__, abgr, 0x55);
				__m128i A_a_ = _mm_and_si128(ABab, _mm_set1_epi16(static_cast<short>(0xFF00)));
				__m128i G_g_ = _mm_and_si128(GRgr, _mm_set1_epi16(static_cast<short>(0xFF00)));
				__m128i R_r_ = _mm_slli_epi16(GRgr, 8);
				__m128i B_b_ = _mm_slli_epi16(ABab, 8);

				// actual alpha-scaling:
				__m128i inv = _mm_set1_epi16(static_cast<short>(0x8081)); // = ceil((1<<(16+7))/255.0)
				G_g_ = _mm_mulhi_epu16(_mm_mulhi_epu16(G_g_, A_a_), inv);
				// shift 7 to the right and 8 to the left, or shift 1 to the left and mask:
				G_g_ = _mm_and_si128(_mm_add_epi16(G_g_, G_g_), _mm_set1_epi16(static_cast<short>(0xFF00)));
				__m128i _R_r = _mm_mulhi_epu16(_mm_mulhi_epu16(R_r_, A_a_), inv);
				_R_r = _mm_srli_epi16(_R_r, 7);
				__m128i _B_b = _mm_mulhi_epu16(_mm_mulhi_epu16(B_b_, A_a_), inv);
				_B_b = _mm_srli_epi16(_B_b, 7);

				// re-assemble channels:
				GRgr = _mm_or_si128(_R_r, G_g_);
				ABab = _mm_or_si128(A_a_, _B_b);

				__m128i __GR = _mm_srli_epi32(GRgr, 16);
				__m128i ab__ = _mm_slli_epi32(ABab, 16);

				ABGR = _mm_blend_epi16(ABab, __GR, 0x55);
				abgr = _mm_blend_epi16(ab__, GRgr, 0x55);

				// store result
				_mm_store_si128(ptr, abgr);
				_mm_store_si128(ptr + 1, ABGR);
			}
		}

		static void PremultiplyAlpha_AVX2(Color* pixels, const size_t num_pixels)
		{
			const size_t loopCount = ((num_pixels + 7) / (sizeof(__m256i) / sizeof(Color)));
			uint32* p = reinterpret_cast<uint32*>(pixels);

			__m256i const mask_alpha_color_odd_255 = _mm256_set1_epi32(static_cast<int>(0xff000000));
			__m256i const div_255 = _mm256_set1_epi16(static_cast<short>(0x8081));

			__m256i const mask_shuffle_alpha = _mm256_set_epi32(0x0f800f80, 0x0b800b80, 0x07800780, 0x03800380, 0x0f800f80, 0x0b800b80, 0x07800780, 0x03800380);
			__m256i const mask_shuffle_color_odd = _mm256_set_epi32(static_cast<int>(0x80800d80), static_cast<int>(0x80800980), static_cast<int>(0x80800580), static_cast<int>(0x80800180), static_cast<int>(0x80800d80), static_cast<int>(0x80800980), static_cast<int>(0x80800580), static_cast<int>(0x80800180));

			__m256i color, alpha, color_even, color_odd;
			for (__m256i* ptr = reinterpret_cast<__m256i*>(p), *end = (ptr + loopCount); ptr != end; ++ptr)
			{
				color = _mm256_load_si256(ptr);

				alpha = _mm256_shuffle_epi8(color, mask_shuffle_alpha);

				color_even = _mm256_slli_epi16(color, 8);
				color_odd = _mm256_shuffle_epi8(color, mask_shuffle_color_odd);
				color_odd = _mm256_or_si256(color_odd, mask_alpha_color_odd_255);

				color_odd = _mm256_mulhi_epu16(color_odd, alpha);
				color_even = _mm256_mulhi_epu16(color_even, alpha);

				color_odd = _mm256_srli_epi16(_mm256_mulhi_epu16(color_odd, div_255), 7);
				color_even = _mm256_srli_epi16(_mm256_mulhi_epu16(color_even, div_255), 7);

				color = _mm256_or_si256(color_even, _mm256_slli_epi16(color_odd, 8));

				_mm256_store_si256(ptr, color);
			}
		}

	# endif

		//
		////////////////////////////////////////////////////////////////

		static void BGRAtoRGBA_plain(Color* pixels, const size_t num_pixels)
		{
			Color* p = pixels;
			const Color* const pEnd = (pixels + num_pixels);

			while (p != pEnd)
			{
				const uint8 r = p->r;
				p->r = p->b;
				p->b = r;
				++p;
			}
		}

	# if SIV3D_INTRINSIC(SSE)

		static void BGRAtoRGBA_SSSE3(Color* pixels, const size_t num_pixels)
		{
			const size_t loopCount = (((num_pixels + 7) / 8) * 2);
			const __m128i mask = ::_mm_set_epi8(15, 12, 13, 14, 11, 8, 9, 10, 7, 4, 5, 6, 3, 0, 1, 2);
			
			for (__m128i* ptr = reinterpret_cast<__m128i*>(pixels), *end = (ptr + loopCount); ptr != end; ptr += 2)
			{
				__m128i t1 = ::_mm_load_si128(ptr);
				__m128i t2 = ::_mm_load_si128(ptr + 1);

				t1 = ::_mm_shuffle_epi8(t1, mask);
				t2 = ::_mm_shuffle_epi8(t2, mask);
				
				::_mm_store_si128(ptr, t1);
				::_mm_store_si128(ptr + 1, t2);
			}
		}

		static void BGRAtoRGBA_AVX2(Color* pixels, const size_t num_pixels)
		{
			const size_t loopCount = ((num_pixels + 7) / 8);
			const __m256i mask = ::_mm256_setr_epi8(
				2, 1, 0, 3, 6, 5, 4, 7, 
				10, 9, 8, 11, 14, 13, 12, 15,
				18, 17, 16, 19, 22, 21, 20, 23,
				26, 25, 24, 27, 30, 29, 28, 31
			);

			for (__m256i* ptr = reinterpret_cast<__m256i*>(pixels), *end = (ptr + loopCount); ptr != end; ++ptr)
			{
				__m256i t = ::_mm256_load_si256(ptr);
				t = ::_mm256_shuffle_epi8(t, mask);
				::_mm256_store_si256(ptr, t);
			}
		}

	# endif
			
	# if SIV3D_INTRINSIC(NEON)

		static void BGRAtoRGBA_NEON(Color* pixels, const size_t num_pixels)
		{
			const size_t loopCount = ((num_pixels + 7) / 8);
			uint8* ptr = reinterpret_cast<uint8*>(pixels);
			const uint8* const end = reinterpret_cast<const uint8*>(pixels + loopCount * 8);
			
			for (; ptr != end; ptr += (8 * 4))
			{
				uint8x8x4_t bgra = vld4_u8(reinterpret_cast<const uint8_t*>(ptr));

				uint8x8x4_t rgba;
				rgba.val[0] = bgra.val[2];
				rgba.val[1] = bgra.val[1];
				rgba.val[2] = bgra.val[0];
				rgba.val[3] = bgra.val[3];

				vst4_u8(reinterpret_cast<uint8_t*>(ptr), rgba);
			}
		}
			
	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	premultiplyAlpha
	//
	////////////////////////////////////////////////////////////////

	void Image::premultiplyAlpha(const bool useSIMD)
	{
	# if SIV3D_CPU(X86_64)

		if (useSIMD)
		{
			if (SupportsAVX2())
			{
				return PremultiplyAlpha_AVX2(m_pixels.data(), m_pixels.size());
			}
			else
			{
				return PremultiplyAlpha_SSE41(m_pixels.data(), m_pixels.size());
			}
		}

	# elif SIV3D_PLATFORM(MACOS)

		if (useSIMD)
		{
			return PremultiplyAlpha_GCC(m_pixels.data(), m_pixels.size());
		}
		
	# endif

		return PremultiplyAlpha_plain(m_pixels.data(), m_pixels.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	unpremultiplyAlpha
	//
	////////////////////////////////////////////////////////////////

	void Image::unpremultiplyAlpha()
	{
		return UnmultiplyAlpha_plain(m_pixels.data(), m_pixels.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	swapBGRAtoRGBA
	//
	////////////////////////////////////////////////////////////////

	void Image::bgraToRGBA(const bool useSIMD)
	{
	# if SIV3D_INTRINSIC(SSE)

		if (useSIMD)
		{
			if (SupportsAVX2())
			{
				return BGRAtoRGBA_AVX2(m_pixels.data(), m_pixels.size());
			}
			else
			{
				return BGRAtoRGBA_SSSE3(m_pixels.data(), m_pixels.size());
			}
		}

	# elif SIV3D_INTRINSIC(NEON)

		if (useSIMD)
		{
			return BGRAtoRGBA_NEON(m_pixels.data(), m_pixels.size());
		}
		
	# endif

		return BGRAtoRGBA_plain(m_pixels.data(), m_pixels.size());
	}
}

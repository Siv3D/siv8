//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Image.hpp>
# include <Siv3D/CPUInfo.hpp>

namespace s3d
{
	namespace
	{
		static void PremultiplyAlpha_plain(Color* pixels, const size_t num_pixels)
		{
			Color* p = pixels;
			const Color* const pEnd = (pixels + num_pixels);

			while (p != pEnd)
			{
				p->r = static_cast<uint8>((static_cast<uint16>(p->r) * p->a) / 255);
				p->g = static_cast<uint8>((static_cast<uint16>(p->g) * p->a) / 255);
				p->b = static_cast<uint8>((static_cast<uint16>(p->b) * p->a) / 255);
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

	# if SIV3D_CPU(X86_64)

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
	}

	////////////////////////////////////////////////////////////////
	//
	//	premultiplyAlpha
	//
	////////////////////////////////////////////////////////////////

	void Image::premultiplyAlpha(const bool useSIMD)
	{
	# if SIV3D_PLATFORM(MACOS)

		if (useSIMD)
		{
			return PremultiplyAlpha_GCC(m_pixels.data(), m_pixels.size());
		}
		else
		{
			return PremultiplyAlpha_plain(m_pixels.data(), m_pixels.size());
		}
		
	# endif

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

	# endif

		return PremultiplyAlpha_plain(m_pixels.data(), m_pixels.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	swapBGRAtoRGBA
	//
	////////////////////////////////////////////////////////////////

	void Image::swapBGRAtoRGBA(const bool useSIMD)
	{
		// [Siv3D ToDo]
		for (auto& pixel : m_pixels)
		{
			std::swap(pixel.r, pixel.b);
		}
	}
}

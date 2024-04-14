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
# include <Siv3D/ImageDecoder.hpp>
# include <Siv3D/ImageEncoder.hpp>
# include <Siv3D/CPUInfo.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Image::Image(const size_t size, Arg::generator_<FunctionRef<Color()>> generator)
		: Image{ Size{ size, size }, generator } {}

	Image::Image(const size_t size, Arg::generator_<FunctionRef<Color(int32, int32)>> generator)
		: Image{ Size{ size, size }, generator } {}

	Image::Image(const size_t size, Arg::generator_<FunctionRef<Color(Point)>> generator)
		: Image{ Size{ size, size }, generator } {}

	Image::Image(const size_t size, Arg::generator0_1_<FunctionRef<Color(double, double)>> generator)
		: Image{ Size{ size, size }, generator } {}

	Image::Image(const size_t size, Arg::generator0_1_<FunctionRef<Color(Vec2)>> generator)
		: Image{ Size{ size, size }, generator } {}

	Image::Image(const size_t width, const  size_t height, Arg::generator_<FunctionRef<Color()>> generator)
		: Image{ Size{ width, height }, generator } {}

	Image::Image(const size_t width, const size_t height, Arg::generator_<FunctionRef<Color(int32, int32)>> generator)
		: Image{ Size{ width, height }, generator } {}

	Image::Image(const size_t width, const size_t height, Arg::generator_<FunctionRef<Color(Point)>> generator)
		: Image{ Size{ width, height }, generator } {}

	Image::Image(const size_t width, const size_t height, Arg::generator0_1_<FunctionRef<Color(double, double)>> generator)
		: Image{ Size{ width, height }, generator } {}

	Image::Image(const size_t width, const size_t height, Arg::generator0_1_<FunctionRef<Color(Vec2)>> generator)
		: Image{ Size{ width, height }, generator } {}

	Image::Image(const Size size, Arg::generator_<FunctionRef<Color()>> generator)
		: Image{ size }
	{
		const auto &f = *generator;

		Color* pDst = m_pixels.data();
		const Color* const pDstEnd = (pDst + m_pixels.size());

		while (pDst != pDstEnd)
		{
			*pDst++ = f();
		}
	}

	Image::Image(const Size size, Arg::generator_<FunctionRef<Color(int32, int32)>> generator)
		: Image{ size }
	{
		const auto& f = *generator;

		Color* pDst = m_pixels.data();

		for (int32 y = 0; y < m_size.y; ++y)
		{
			for (int32 x = 0; x < m_size.x; ++x)
			{
				*pDst++ = f(x, y);
			}
		}
	}

	Image::Image(const Size size, Arg::generator_<FunctionRef<Color(Point)>> generator)
		: Image{ size }
	{
		const auto& f = *generator;

		Color* pDst = m_pixels.data();

		for (int32 y = 0; y < m_size.y; ++y)
		{
			for (int32 x = 0; x < m_size.x; ++x)
			{
				*pDst++ = f(Point{ x, y });
			}
		}
	}

	Image::Image(const Size size, Arg::generator0_1_<FunctionRef<Color(double, double)>> generator)
		: Image{ size }
	{
		const auto& f = *generator;

		Color* pDst = m_pixels.data();

		const double dx = (1.0 / m_size.x);
		const double dy = (1.0 / m_size.y);
		const double offsetX = (dx * 0.5);
		const double offsetY = (dy * 0.5);

		for (int32 y = 0; y < m_size.y; ++y)
		{
			for (int32 x = 0; x < m_size.x; ++x)
			{
				*pDst++ = f((x * dx + offsetX), (y * dy + offsetY));
			}
		}
	}

	Image::Image(const Size size, Arg::generator0_1_<FunctionRef<Color(Vec2)>> generator)
		: Image{ size }
	{
		const auto& f = *generator;

		Color* pDst = m_pixels.data();

		const double dx = (1.0 / m_size.x);
		const double dy = (1.0 / m_size.y);
		const double offsetX = (dx * 0.5);
		const double offsetY = (dy * 0.5);

		for (int32 y = 0; y < m_size.y; ++y)
		{
			for (int32 x = 0; x < m_size.x; ++x)
			{
				*pDst++ = f(Vec2{ (x * dx + offsetX), (y * dy + offsetY) });
			}
		}
	}

	Image::Image(const FilePathView path, const ImageFormat format)
	{
		*this = ImageDecoder::Decode(path, format);
	}

	Image::Image(IReader&& reader, const ImageFormat format)
	{
		*this = ImageDecoder::Decode(reader, format);
	}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	void Image::fill(const Color color) noexcept
	{
		std::fill(m_pixels.begin(), m_pixels.end(), color); // 明示的に SIMD にしなくても早いコードが生成される
	}

	////////////////////////////////////////////////////////////////
	//
	//	resize
	//
	////////////////////////////////////////////////////////////////

	void Image::resize(const size_t width, const size_t height)
	{
		resize(Size{ width, height });
	}

	void Image::resize(const Size size)
	{
		m_size = ValidImageSizeOrEmpty(size);

		m_pixels.resize(m_size.area());
	}

	void Image::resize(const size_t width, const size_t height, const Color fillColor)
	{
		resize(Size{ width, height }, fillColor);
	}

	void Image::resize(const Size size, const Color fillColor)
	{
		m_size = ValidImageSizeOrEmpty(size);

		m_pixels.assign((m_size.area()), fillColor);
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeHeight
	//
	////////////////////////////////////////////////////////////////

	void Image::resizeHeight(const size_t height)
	{
		resizeHeight(height, Color::Zero());
	}

	void Image::resizeHeight(const size_t height, const Color fillColor)
	{
		const int32 oldHeight = m_size.y;

		// 高さが変更されない場合は何もしない
		if (oldHeight == m_size.y)
		{
			return;
		}

		m_size = ValidImageSizeOrEmpty({ m_size.x, height });

		if (m_size.y < oldHeight) // 高さが減る場合
		{
			m_pixels.resize(m_size.area());
		}
		else // 高さが増える場合
		{
			m_pixels.append((m_size.x * (m_size.y - oldHeight)), fillColor);
		}
	}





	////////////////////////////////////////////////////////////////
	//
	//	premultiplyAlpha
	//
	////////////////////////////////////////////////////////////////

	void Image::premultiplyAlpha()
	{
		Color* p = m_pixels.data();
		const Color* const pEnd = (p + m_pixels.size());

		while (p != pEnd)
		{
			p->r = static_cast<uint8>((static_cast<uint16>(p->r) * p->a) / 255);
			p->g = static_cast<uint8>((static_cast<uint16>(p->g) * p->a) / 255);
			p->b = static_cast<uint8>((static_cast<uint16>(p->b) * p->a) / 255);
			++p;
		}
	}

	// https://github.com/Wizermil/premultiply_alpha
	void Image::premultiplyAlpha_sse41()
	{
	# if SIV3D_CPU(X86_64)

		const size_t loopCount = ((m_pixels.size() + 3) / (sizeof(__m128i) / sizeof(Color)) / 2);
		uint32* p = reinterpret_cast<uint32*>(m_pixels.data());

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

	# else

		premultiplyAlpha();

	# endif
	}

	// https://github.com/Wizermil/premultiply_alpha
	void Image::premultiplyAlpha_avx2()
	{
	# if SIV3D_CPU(X86_64)

		if (SupportsAVX2())
		{
			const size_t loopCount = ((m_pixels.size() + 7) / (sizeof(__m256i) / sizeof(Color)));
			uint32* p = reinterpret_cast<uint32*>(m_pixels.data());

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
		else
		{
			premultiplyAlpha_sse41();
		}

	# else

		premultiplyAlpha();

	# endif
	}

	// https://github.com/Wizermil/premultiply_alpha
	void Image::premultiplyAlpha_gcc()
	{
	# if (SIV3D_COMPILER(GCC) || SIV3D_COMPILER(CLANG))

		using u8x16 = __attribute__((ext_vector_type(16))) uint8_t;
		using u16x8 = __attribute__((ext_vector_type(8))) uint16_t;

		size_t const loopCount = ((m_pixels.size() + 3) / 4);

		for (u8x16* ptr = reinterpret_cast<u8x16*>(data), *end = (ptr + loopCount); ptr != end; ++ptr)
		{
			u16x8 const alpha{ ptr[3], ptr[3], ptr[7], ptr[7], ptr[11], ptr[11], ptr[15], ptr[15] };
			u16x8 color_even{ (reinterpret_cast<u16x8>(ptr) >> 8) | u16x8{ 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF, 0x0000, 0x00FF } };
			u16x8 color_odd{ reinterpret_cast<u16x8>(ptr) & u16x8 { 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF, 0x00FF } };
			color_even = color_even * alpha;
			color_odd = color_odd * alpha;
			color_even /= 255U;
			color_odd /= 255U;
			*ptr = reinterpret_cast<u8x16>((color_even << 8) | color_odd);
		}

	# else

		premultiplyAlpha();

	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool Image::save(const FilePathView path, const ImageFormat format) const
	{
		return ImageEncoder::Save(*this, format, path);
	}

}

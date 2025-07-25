﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Image.hpp>
# include <Siv3D/Emoji.hpp>
# include <Siv3D/ImageDecoder.hpp>
# include <Siv3D/ImageEncoder.hpp>
# include "ImagePainting.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Image::Image(const Concept::Integral auto size, Arg::generator_<FunctionRef<Color()>> generator)
		: Image{ Size{ size, size }, generator } {}

	Image::Image(const Concept::Integral auto size, Arg::generator_<FunctionRef<Color(int32, int32)>> generator)
		: Image{ Size{ size, size }, generator } {}

	Image::Image(const Concept::Integral auto size, Arg::generator_<FunctionRef<Color(Point)>> generator)
		: Image{ Size{ size, size }, generator } {}

	Image::Image(const Concept::Integral auto size, Arg::generator0_1_<FunctionRef<Color(double, double)>> generator)
		: Image{ Size{ size, size }, generator } {}

	Image::Image(const Concept::Integral auto size, Arg::generator0_1_<FunctionRef<Color(Vec2)>> generator)
		: Image{ Size{ size, size }, generator } {}

	Image::Image(const Concept::Integral auto width, const Concept::Integral auto height, Arg::generator_<FunctionRef<Color()>> generator)
		: Image{ Size{ width, height }, generator } {}

	Image::Image(const Concept::Integral auto width, const Concept::Integral auto height, Arg::generator_<FunctionRef<Color(int32, int32)>> generator)
		: Image{ Size{ width, height }, generator } {}

	Image::Image(const Concept::Integral auto width, const Concept::Integral auto height, Arg::generator_<FunctionRef<Color(Point)>> generator)
		: Image{ Size{ width, height }, generator } {}

	Image::Image(const Concept::Integral auto width, const Concept::Integral auto height, Arg::generator0_1_<FunctionRef<Color(double, double)>> generator)
		: Image{ Size{ width, height }, generator } {}

	Image::Image(const Concept::Integral auto width, const Concept::Integral auto height, Arg::generator0_1_<FunctionRef<Color(Vec2)>> generator)
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

	Image::Image(const FilePathView path, const PremultiplyAlpha premultiplyAlpha, const ImageFormat format)
	{
		*this = ImageDecoder::Decode(path, premultiplyAlpha, format);
	}

	Image::Image(IReader&& reader, const PremultiplyAlpha premultiplyAlpha, const ImageFormat format)
	{
		*this = ImageDecoder::Decode(reader, premultiplyAlpha, format);
	}

	Image::Image(const FilePathView rgb, const FilePathView alpha, const PremultiplyAlpha premultiplyAlpha)
		: Image{ rgb, PremultiplyAlpha::No }
	{
		const Image alphaImage{ alpha, PremultiplyAlpha::No };

		if (m_size != alphaImage.size())
		{
			release();
			return;
		}

		{
			const Color* pSrcAlpha = alphaImage.data();
			Color* pDst = m_pixels.data();
			const Color* const pDstEnd = (pDst + m_pixels.size());

			while (pDst != pDstEnd)
			{
				pDst->a = pSrcAlpha->r;
				++pSrcAlpha;
				++pDst;
			}
		}

		if (premultiplyAlpha == PremultiplyAlpha::Yes)
		{
			this->premultiplyAlpha();
		}
	}

	Image::Image(const Color rgb, const FilePathView alpha, const PremultiplyAlpha premultiplyAlpha)
		: Image{ alpha, PremultiplyAlpha::No }
	{
		for (auto& pixel : *this)
		{
			pixel.a = pixel.r;
			pixel.r = rgb.r;
			pixel.g = rgb.g;
			pixel.b = rgb.b;
		}

		if (premultiplyAlpha == PremultiplyAlpha::Yes)
		{
			this->premultiplyAlpha();
		}
	}

	Image::Image(const Emoji& emoji, const int32 size)
	{
		*this = Emoji::CreateImage(emoji.codePoints, size);
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
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool Image::save(const FilePathView path, const ImageFormat format) const
	{
		return ImageEncoder::Save(*this, format, path);
	}

	

	////////////////////////////////////////////////////////////////
	//
	//	overwrite
	//
	////////////////////////////////////////////////////////////////

	void Image::overwrite(Image& dst, const Point pos) const
	{
		if (this == &dst)
		{
			return;
		}

		const Image& src = *this;

		const int32 dstXBegin = Max(pos.x, 0);
		const int32 dstYBegin = Max(pos.y, 0);
		const int32 dstXEnd = Min(pos.x + src.width(), dst.width());
		const int32 dstYEnd = Min(pos.y + src.height(), dst.height());
		const int32 writeWidth = ((dstXEnd - dstXBegin) > 0 ? (dstXEnd - dstXBegin) : 0);
		const int32 writeHeight = ((dstYEnd - dstYBegin) > 0 ? (dstYEnd - dstYBegin) : 0);

		if ((writeWidth * writeHeight) == 0)
		{
			return;
		}

		const int32 srcXBegin = Max(0, -pos.x);
		const int32 srcYBegin = Max(0, -pos.y);
		const Color* pSrc = &src[srcYBegin][srcXBegin];
		Color* pDst = &dst[dstYBegin][dstXBegin];
		const int32 srcWidth = src.width();
		const int32 dstWidth = dst.width();

		ImagePainting::Overwrite(pSrc, pDst, writeWidth, writeHeight, srcWidth, dstWidth);
	}
}

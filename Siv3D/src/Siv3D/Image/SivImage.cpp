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
# include <Siv3D/Emoji.hpp>
# include <Siv3D/ImageDecoder.hpp>
# include <Siv3D/ImageEncoder.hpp>

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










	Image& Image::rotate90()
	{
		// 1. パラメータチェック
		{
			if (isEmpty())
			{
				return *this;
			}
		}

		// 2. 処理
		{
			// [Siv3D ToDo] 最適化
			Image tmp{ m_size.y, m_size.x };

			for (size_t y = 0; y < m_size.y; ++y)
			{
				const Color* pSrc = data() + y * m_size.x;
				const size_t dstX = m_size.y - y - 1;

				for (size_t x = 0; x < m_size.x; ++x)
				{
					tmp[x][dstX] = pSrc[x];
				}
			}

			swap(tmp);
		}

		return *this;
	}

	Image& Image::rotate90_4()
	{
		// 1. パラメータチェック
		{
			if (isEmpty())
			{
				return *this;
			}
		}

		// 2. 処理
		{
			constexpr size_t BlockSize = 4;

			if (m_size.x == m_size.y)
			{
				Color* const pData = data();
				const size_t halfHeight = (m_size.y / 2);
				const size_t halfWidth = (halfHeight + m_size.y % 2);

				// 以下のように画像を4つの領域 A, B, C, D に分け、各画素を A←B, B←C, C←D, D←A とそれぞれコピーする
				//     AADD    AAADD
				//     AADD    AAADD
				//     BBCC    BB.DD
				//     BBCC    BBCCC
				//             BBCCC
				for (size_t b = 0; b < halfWidth; b += BlockSize)
				{
					for (size_t y = 0; y < halfHeight; ++y)
					{
						Color* p1 = (pData + y * m_size.x + b);
						Color* p2 = (pData + (m_size.y - b - 1) * m_size.x + y);
						Color* p3 = (pData + (m_size.y - y) * m_size.x - b - 1);
						Color* p4 = (pData + (b + 1) * m_size.x - y - 1);
						const size_t w = Min((halfWidth - b), BlockSize);

						for (size_t x = 0; x < w; ++x)
						{
							const Color tmp = *p1;
							*p1 = *p2;
							*p2 = *p3;
							*p3 = *p4;
							*p4 = tmp;
							++p1;
							p2 -= m_size.x;
							--p3;
							p4 += m_size.x;
						}
					}
				}
			}
			else
			{
				Image tmp{ m_size.y, m_size.x };

				const Color* const pData = data();
				Color* const pDstBase = tmp.data();

				for (size_t b = 0; b < m_size.x; b += BlockSize)
				{
					Color* const pDstLine = (pDstBase + b * m_size.y);
					const size_t w = Min((m_size.x - b), BlockSize);

					for (size_t y = 0; y < m_size.y; ++y)
					{
						const Color* pSrc = (pData + y * m_size.x + b);
						Color* pDst = (pDstLine + (m_size.y - y - 1));

						for (size_t x = 0; x < w; ++x)
						{
							*pDst = *pSrc;
							++pSrc;
							pDst += m_size.y;
						}
					}
				}

				swap(tmp);
			}
		}

		return *this;
	}

	Image& Image::rotate90_8()
	{
		// 1. パラメータチェック
		{
			if (isEmpty())
			{
				return *this;
			}
		}

		// 2. 処理
		{
			constexpr size_t BlockSize = 8;

			if (m_size.x == m_size.y)
			{
				Color* const pData = data();
				const size_t halfHeight = (m_size.y / 2);
				const size_t halfWidth = (halfHeight + m_size.y % 2);

				// 以下のように画像を4つの領域 A, B, C, D に分け、各画素を A←B, B←C, C←D, D←A とそれぞれコピーする
				//     AADD    AAADD
				//     AADD    AAADD
				//     BBCC    BB.DD
				//     BBCC    BBCCC
				//             BBCCC
				for (size_t b = 0; b < halfWidth; b += BlockSize)
				{
					for (size_t y = 0; y < halfHeight; ++y)
					{
						Color* p1 = (pData + y * m_size.x + b);
						Color* p2 = (pData + (m_size.y - b - 1) * m_size.x + y);
						Color* p3 = (pData + (m_size.y - y) * m_size.x - b - 1);
						Color* p4 = (pData + (b + 1) * m_size.x - y - 1);
						const size_t w = Min((halfWidth - b), BlockSize);

						for (size_t x = 0; x < w; ++x)
						{
							const Color tmp = *p1;
							*p1 = *p2;
							*p2 = *p3;
							*p3 = *p4;
							*p4 = tmp;
							++p1;
							p2 -= m_size.x;
							--p3;
							p4 += m_size.x;
						}
					}
				}
			}
			else
			{
				Image tmp{ m_size.y, m_size.x };

				const Color* const pData = data();
				Color* const pDstBase = tmp.data();

				for (size_t b = 0; b < m_size.x; b += BlockSize)
				{
					Color* const pDstLine = (pDstBase + b * m_size.y);
					const size_t w = Min((m_size.x - b), BlockSize);

					for (size_t y = 0; y < m_size.y; ++y)
					{
						const Color* pSrc = (pData + y * m_size.x + b);
						Color* pDst = (pDstLine + (m_size.y - y - 1));

						for (size_t x = 0; x < w; ++x)
						{
							*pDst = *pSrc;
							++pSrc;
							pDst += m_size.y;
						}
					}
				}

				swap(tmp);
			}
		}

		return *this;
	}

	Image& Image::rotate90_16()
	{
		// 1. パラメータチェック
		{
			if (isEmpty())
			{
				return *this;
			}
		}

		// 2. 処理
		{
			constexpr size_t BlockSize = 16;

			if (m_size.x == m_size.y)
			{
				Color* const pData = data();
				const size_t halfHeight = (m_size.y / 2);
				const size_t halfWidth = (halfHeight + m_size.y % 2);

				// 以下のように画像を4つの領域 A, B, C, D に分け、各画素を A←B, B←C, C←D, D←A とそれぞれコピーする
				//     AADD    AAADD
				//     AADD    AAADD
				//     BBCC    BB.DD
				//     BBCC    BBCCC
				//             BBCCC
				for (size_t b = 0; b < halfWidth; b += BlockSize)
				{
					for (size_t y = 0; y < halfHeight; ++y)
					{
						Color* p1 = (pData + y * m_size.x + b);
						Color* p2 = (pData + (m_size.y - b - 1) * m_size.x + y);
						Color* p3 = (pData + (m_size.y - y) * m_size.x - b - 1);
						Color* p4 = (pData + (b + 1) * m_size.x - y - 1);
						const size_t w = Min((halfWidth - b), BlockSize);

						for (size_t x = 0; x < w; ++x)
						{
							const Color tmp = *p1;
							*p1 = *p2;
							*p2 = *p3;
							*p3 = *p4;
							*p4 = tmp;
							++p1;
							p2 -= m_size.x;
							--p3;
							p4 += m_size.x;
						}
					}
				}
			}
			else
			{
				Image tmp{ m_size.y, m_size.x };

				const Color* const pData = data();
				Color* const pDstBase = tmp.data();

				for (size_t b = 0; b < m_size.x; b += BlockSize)
				{
					Color* const pDstLine = (pDstBase + b * m_size.y);
					const size_t w = Min((m_size.x - b), BlockSize);

					for (size_t y = 0; y < m_size.y; ++y)
					{
						const Color* pSrc = (pData + y * m_size.x + b);
						Color* pDst = (pDstLine + (m_size.y - y - 1));

						for (size_t x = 0; x < w; ++x)
						{
							*pDst = *pSrc;
							++pSrc;
							pDst += m_size.y;
						}
					}
				}

				swap(tmp);
			}
		}

		return *this;
	}

	Image& Image::rotate90_32()
	{
		// 1. パラメータチェック
		{
			if (isEmpty())
			{
				return *this;
			}
		}

		// 2. 処理
		{
			constexpr size_t BlockSize = 32;

			if (m_size.x == m_size.y)
			{
				Color* const pData = data();
				const size_t halfHeight = (m_size.y / 2);
				const size_t halfWidth = (halfHeight + m_size.y % 2);

				// 以下のように画像を4つの領域 A, B, C, D に分け、各画素を A←B, B←C, C←D, D←A とそれぞれコピーする
				//     AADD    AAADD
				//     AADD    AAADD
				//     BBCC    BB.DD
				//     BBCC    BBCCC
				//             BBCCC
				for (size_t b = 0; b < halfWidth; b += BlockSize)
				{
					for (size_t y = 0; y < halfHeight; ++y)
					{
						Color* p1 = (pData + y * m_size.x + b);
						Color* p2 = (pData + (m_size.y - b - 1) * m_size.x + y);
						Color* p3 = (pData + (m_size.y - y) * m_size.x - b - 1);
						Color* p4 = (pData + (b + 1) * m_size.x - y - 1);
						const size_t w = Min((halfWidth - b), BlockSize);

						for (size_t x = 0; x < w; ++x)
						{
							const Color tmp = *p1;
							*p1 = *p2;
							*p2 = *p3;
							*p3 = *p4;
							*p4 = tmp;
							++p1;
							p2 -= m_size.x;
							--p3;
							p4 += m_size.x;
						}
					}
				}
			}
			else
			{
				Image tmp{ m_size.y, m_size.x };

				const Color* const pData = data();
				Color* const pDstBase = tmp.data();

				for (size_t b = 0; b < m_size.x; b += BlockSize)
				{
					Color* const pDstLine = (pDstBase + b * m_size.y);
					const size_t w = Min((m_size.x - b), BlockSize);

					for (size_t y = 0; y < m_size.y; ++y)
					{
						const Color* pSrc = (pData + y * m_size.x + b);
						Color* pDst = (pDstLine + (m_size.y - y - 1));

						for (size_t x = 0; x < w; ++x)
						{
							*pDst = *pSrc;
							++pSrc;
							pDst += m_size.y;
						}
					}
				}

				swap(tmp);
			}
		}

		return *this;
	}

	Image Image::rotated90() const&
	{
		// 1. パラメータチェック
		{
			if (isEmpty())
			{
				return *this;
			}
		}

		// 2. 処理
		{
			Image image{ m_size.y, m_size.x };

			// [Siv3D ToDo] 最適化
			for (size_t y = 0; y < m_size.y; ++y)
			{
				const Color* pSrc = data() + y * m_size.x;
				const size_t dstX = m_size.y - y - 1;

				for (size_t x = 0; x < m_size.x; ++x)
				{
					image[x][dstX] = pSrc[x];
				}
			}

			return image;
		}
	}

	Image Image::rotated90_4() const&
	{
		// 1. パラメータチェック
		{
			if (isEmpty())
			{
				return *this;
			}
		}

		// 2. 処理
		{
			constexpr size_t BlockSize = 4;

			if (m_size.x == m_size.y)
			{
				Image image = *this;

				Color* const pData = image.data();
				const size_t halfHeight = (m_size.y / 2);
				const size_t halfWidth = (halfHeight + m_size.y % 2);

				// 以下のように画像を4つの領域 A, B, C, D に分け、各画素を A←B, B←C, C←D, D←A とそれぞれコピーする
				//     AADD    AAADD
				//     AADD    AAADD
				//     BBCC    BB.DD
				//     BBCC    BBCCC
				//             BBCCC
				for (size_t b = 0; b < halfWidth; b += BlockSize)
				{
					for (size_t y = 0; y < halfHeight; ++y)
					{
						Color* p1 = (pData + y * m_size.x + b);
						Color* p2 = (pData + (m_size.y - b - 1) * m_size.x + y);
						Color* p3 = (pData + (m_size.y - y) * m_size.x - b - 1);
						Color* p4 = (pData + (b + 1) * m_size.x - y - 1);
						const size_t w = Min((halfWidth - b), BlockSize);

						for (size_t x = 0; x < w; ++x)
						{
							const Color tmp = *p1;
							*p1 = *p2;
							*p2 = *p3;
							*p3 = *p4;
							*p4 = tmp;
							++p1;
							p2 -= m_size.x;
							--p3;
							p4 += m_size.x;
						}
					}
				}

				return image;
			}
			else
			{
				Image image{ m_size.y, m_size.x };

				const Color* const pData = data();
				Color* const pDstBase = image.data();

				for (size_t b = 0; b < m_size.x; b += BlockSize)
				{
					Color* const pDstLine = (pDstBase + b * m_size.y);
					const size_t w = Min((m_size.x - b), BlockSize);

					for (size_t y = 0; y < m_size.y; ++y)
					{
						const Color* pSrc = (pData + y * m_size.x + b);
						Color* pDst = (pDstLine + (m_size.y - y - 1));

						for (size_t x = 0; x < w; ++x)
						{
							*pDst = *pSrc;
							++pSrc;
							pDst += m_size.y;
						}
					}
				}

				return image;
			}
		}
	}

	Image Image::rotated90_8() const&
	{
		// 1. パラメータチェック
		{
			if (isEmpty())
			{
				return *this;
			}
		}

		// 2. 処理
		{
			constexpr size_t BlockSize = 8;

			if (m_size.x == m_size.y)
			{
				Image image = *this;

				Color* const pData = image.data();
				const size_t halfHeight = (m_size.y / 2);
				const size_t halfWidth = (halfHeight + m_size.y % 2);

				// 以下のように画像を4つの領域 A, B, C, D に分け、各画素を A←B, B←C, C←D, D←A とそれぞれコピーする
				//     AADD    AAADD
				//     AADD    AAADD
				//     BBCC    BB.DD
				//     BBCC    BBCCC
				//             BBCCC
				for (size_t b = 0; b < halfWidth; b += BlockSize)
				{
					for (size_t y = 0; y < halfHeight; ++y)
					{
						Color* p1 = (pData + y * m_size.x + b);
						Color* p2 = (pData + (m_size.y - b - 1) * m_size.x + y);
						Color* p3 = (pData + (m_size.y - y) * m_size.x - b - 1);
						Color* p4 = (pData + (b + 1) * m_size.x - y - 1);
						const size_t w = Min((halfWidth - b), BlockSize);

						for (size_t x = 0; x < w; ++x)
						{
							const Color tmp = *p1;
							*p1 = *p2;
							*p2 = *p3;
							*p3 = *p4;
							*p4 = tmp;
							++p1;
							p2 -= m_size.x;
							--p3;
							p4 += m_size.x;
						}
					}
				}

				return image;
			}
			else
			{
				Image image{ m_size.y, m_size.x };

				const Color* const pData = data();
				Color* const pDstBase = image.data();

				for (size_t b = 0; b < m_size.x; b += BlockSize)
				{
					Color* const pDstLine = (pDstBase + b * m_size.y);
					const size_t w = Min((m_size.x - b), BlockSize);

					for (size_t y = 0; y < m_size.y; ++y)
					{
						const Color* pSrc = (pData + y * m_size.x + b);
						Color* pDst = (pDstLine + (m_size.y - y - 1));

						for (size_t x = 0; x < w; ++x)
						{
							*pDst = *pSrc;
							++pSrc;
							pDst += m_size.y;
						}
					}
				}

				return image;
			}
		}
	}

	Image Image::rotated90_16() const&
	{
		// 1. パラメータチェック
		{
			if (isEmpty())
			{
				return *this;
			}
		}

		// 2. 処理
		{
			constexpr size_t BlockSize = 16;

			if (m_size.x == m_size.y)
			{
				Image image = *this;

				Color* const pData = image.data();
				const size_t halfHeight = (m_size.y / 2);
				const size_t halfWidth = (halfHeight + m_size.y % 2);

				// 以下のように画像を4つの領域 A, B, C, D に分け、各画素を A←B, B←C, C←D, D←A とそれぞれコピーする
				//     AADD    AAADD
				//     AADD    AAADD
				//     BBCC    BB.DD
				//     BBCC    BBCCC
				//             BBCCC
				for (size_t b = 0; b < halfWidth; b += BlockSize)
				{
					for (size_t y = 0; y < halfHeight; ++y)
					{
						Color* p1 = (pData + y * m_size.x + b);
						Color* p2 = (pData + (m_size.y - b - 1) * m_size.x + y);
						Color* p3 = (pData + (m_size.y - y) * m_size.x - b - 1);
						Color* p4 = (pData + (b + 1) * m_size.x - y - 1);
						const size_t w = Min((halfWidth - b), BlockSize);

						for (size_t x = 0; x < w; ++x)
						{
							const Color tmp = *p1;
							*p1 = *p2;
							*p2 = *p3;
							*p3 = *p4;
							*p4 = tmp;
							++p1;
							p2 -= m_size.x;
							--p3;
							p4 += m_size.x;
						}
					}
				}

				return image;
			}
			else
			{
				Image image{ m_size.y, m_size.x };

				const Color* const pData = data();
				Color* const pDstBase = image.data();

				for (size_t b = 0; b < m_size.x; b += BlockSize)
				{
					Color* const pDstLine = (pDstBase + b * m_size.y);
					const size_t w = Min((m_size.x - b), BlockSize);

					for (size_t y = 0; y < m_size.y; ++y)
					{
						const Color* pSrc = (pData + y * m_size.x + b);
						Color* pDst = (pDstLine + (m_size.y - y - 1));

						for (size_t x = 0; x < w; ++x)
						{
							*pDst = *pSrc;
							++pSrc;
							pDst += m_size.y;
						}
					}
				}

				return image;
			}
		}
	}

	Image Image::rotated90_32() const&
	{
		// 1. パラメータチェック
		{
			if (isEmpty())
			{
				return *this;
			}
		}

		// 2. 処理
		{
			constexpr size_t BlockSize = 32;

			if (m_size.x == m_size.y)
			{
				Image image = *this;

				Color* const pData = image.data();
				const size_t halfHeight = (m_size.y / 2);
				const size_t halfWidth = (halfHeight + m_size.y % 2);

				// 以下のように画像を4つの領域 A, B, C, D に分け、各画素を A←B, B←C, C←D, D←A とそれぞれコピーする
				//     AADD    AAADD
				//     AADD    AAADD
				//     BBCC    BB.DD
				//     BBCC    BBCCC
				//             BBCCC
				for (size_t b = 0; b < halfWidth; b += BlockSize)
				{
					for (size_t y = 0; y < halfHeight; ++y)
					{
						Color* p1 = (pData + y * m_size.x + b);
						Color* p2 = (pData + (m_size.y - b - 1) * m_size.x + y);
						Color* p3 = (pData + (m_size.y - y) * m_size.x - b - 1);
						Color* p4 = (pData + (b + 1) * m_size.x - y - 1);
						const size_t w = Min((halfWidth - b), BlockSize);

						for (size_t x = 0; x < w; ++x)
						{
							const Color tmp = *p1;
							*p1 = *p2;
							*p2 = *p3;
							*p3 = *p4;
							*p4 = tmp;
							++p1;
							p2 -= m_size.x;
							--p3;
							p4 += m_size.x;
						}
					}
				}

				return image;
			}
			else
			{
				Image image{ m_size.y, m_size.x };

				const Color* const pData = data();
				Color* const pDstBase = image.data();

				for (size_t b = 0; b < m_size.x; b += BlockSize)
				{
					Color* const pDstLine = (pDstBase + b * m_size.y);
					const size_t w = Min((m_size.x - b), BlockSize);

					for (size_t y = 0; y < m_size.y; ++y)
					{
						const Color* pSrc = (pData + y * m_size.x + b);
						Color* pDst = (pDstLine + (m_size.y - y - 1));

						for (size_t x = 0; x < w; ++x)
						{
							*pDst = *pSrc;
							++pSrc;
							pDst += m_size.y;
						}
					}
				}

				return image;
			}
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

}

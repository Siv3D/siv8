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

namespace s3d
{
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
		const size_t oldHeight = m_size.y;

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
}

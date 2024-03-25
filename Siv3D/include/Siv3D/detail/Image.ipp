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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	inline Image::Image(Image&& image) noexcept
		: m_pixels(std::move(image.m_pixels))
		, m_size{ image.m_size }
	{
		image.m_size.clear();
	}

	inline Image::Image(const size_t size)
		: Image{ size, size } {}

	inline Image::Image(const size_t size, const Color color)
		: Image{ size, size, color } {}

	inline Image::Image(const size_t width, const size_t height)
		: Image{ Size{ width, height } } {}

	inline Image::Image(const size_t width, const size_t height, const Color color)
		: Image{ Size{ width, height }, color } {}

	inline Image::Image(const Size size)
		: m_size{ ClampImageSize(size) }
		, m_pixels(m_size.area()) {}

	inline Image::Image(const Size size, const Color color)
		: m_size{ ClampImageSize(size) }
		, m_pixels(m_size.area(), color) {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	Image& Image::operator =(Image&& image) noexcept
	{
		if (&image != this)
		{
			m_pixels	= std::move(image.m_pixels);
			m_size		= image.m_size;
			image.m_size.clear();
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	width, height
	//
	////////////////////////////////////////////////////////////////

	int32 Image::width() const noexcept
	{
		return m_size.x;
	}

	int32 Image::height() const noexcept
	{
		return m_size.y;
	}

	////////////////////////////////////////////////////////////////
	//
	//	stride
	//
	////////////////////////////////////////////////////////////////

	size_t Image::stride() const noexcept
	{
		return (m_size.x * sizeof(Color));
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	Size Image::size() const noexcept
	{
		return m_size;
	}

	////////////////////////////////////////////////////////////////
	//
	//	size_bytes
	//
	////////////////////////////////////////////////////////////////

	size_t Image::size_bytes() const noexcept
	{
		return (stride() * m_size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_pixels
	//
	////////////////////////////////////////////////////////////////

	size_t Image::num_pixels() const noexcept
	{
		return (m_size.x * m_size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool Image::isEmpty() const noexcept
	{
		return m_pixels.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	Image::operator bool() const noexcept
	{
		return (not m_pixels.empty());
	}




	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	constexpr Size Image::ClampImageSize(const Size size) noexcept
	{
		if ((size.x <= 0) || (size.y <= 0))
		{
			return{ 0, 0 };
		}

		return{ Min(size.x, MaxWidth), Min(size.y, MaxHeight) };
	}
}

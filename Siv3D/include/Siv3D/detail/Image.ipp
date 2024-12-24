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
		: m_size{ image.m_size }
		, m_pixels(std::move(image.m_pixels))
	{
		image.m_size.clear();
	}

	inline Image::Image(const Concept::Integral auto size)
		: Image{ size, size } {}

	inline Image::Image(const Concept::Integral auto size, const Color color)
		: Image{ size, size, color } {}

	inline Image::Image(const Concept::Integral auto width, const Concept::Integral auto height)
		: Image{ Size{ width, height } } {}

	inline Image::Image(const Concept::Integral auto width, const Concept::Integral auto height, const Color color)
		: Image{ Size{ width, height }, color } {}

	inline Image::Image(const Size size)
		: m_size{ ValidImageSizeOrEmpty(size) }
		, m_pixels(m_size.area()) {}

	inline Image::Image(const Size size, const Color color)
		: m_size{ ValidImageSizeOrEmpty(size) }
		, m_pixels(m_size.area(), color) {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	inline Image& Image::operator =(Image&& image) noexcept
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

	inline int32 Image::width() const noexcept
	{
		return m_size.x;
	}

	inline int32 Image::height() const noexcept
	{
		return m_size.y;
	}

	////////////////////////////////////////////////////////////////
	//
	//	stride
	//
	////////////////////////////////////////////////////////////////

	inline uint32 Image::stride() const noexcept
	{
		return static_cast<uint32>(m_size.x * sizeof(Color));
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	inline Size Image::size() const noexcept
	{
		return m_size;
	}

	////////////////////////////////////////////////////////////////
	//
	//	size_bytes
	//
	////////////////////////////////////////////////////////////////

	inline size_t Image::size_bytes() const noexcept
	{
		return (stride() * m_size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_pixels
	//
	////////////////////////////////////////////////////////////////

	inline size_t Image::num_pixels() const noexcept
	{
		return (m_size.x * m_size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	inline bool Image::isEmpty() const noexcept
	{
		return m_pixels.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	inline Image::operator bool() const noexcept
	{
		return (not m_pixels.empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	horizontalAspectRatio
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Float Image::horizontalAspectRatio() const noexcept
	{
		return m_size.horizontalAspectRatio<Float>();
	}

	////////////////////////////////////////////////////////////////
	//
	//	shrink_to_fit
	//
	////////////////////////////////////////////////////////////////

	inline void Image::shrink_to_fit()
	{
		m_pixels.shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	inline void Image::clear() noexcept
	{
		m_pixels.clear();
		m_size.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	inline void Image::release()
	{
		clear();
		shrink_to_fit();
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	inline void Image::swap(Image& image) noexcept
	{
		m_pixels.swap(image.m_pixels);
		std::ranges::swap(m_size, image.m_size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	cloned
	//
	////////////////////////////////////////////////////////////////

	inline Image Image::cloned() const&
	{
		return *this;
	}

	inline Image Image::cloned()&&
	{
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	indexInBounds
	//
	////////////////////////////////////////////////////////////////

	inline bool Image::indexInBounds(const int64 y, const int64 x) const noexcept
	{
		return ((0 <= y) && (y < static_cast<int64>(m_size.y))
			 && (0 <= x) && (x < static_cast<int64>(m_size.x)));
	}

	inline bool Image::indexInBounds(const Point pos) const noexcept
	{
		return ((0 <= pos.y) && (pos.y < static_cast<int64>(m_size.y))
			 && (0 <= pos.x) && (pos.x < static_cast<int64>(m_size.x)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	inline Color* Image::operator [](const size_t y)
	{
		return (m_pixels.data() + (m_size.x * y));
	}

	inline const Color* Image::operator [](const size_t y) const
	{
		return (m_pixels.data() + (m_size.x * y));
	}

	inline Color& Image::operator [](const Point pos)&
	{
		return *(m_pixels.data() + (m_size.x * pos.y + pos.x));
	}

	inline const Color& Image::operator [](const Point pos) const&
	{
		return *(m_pixels.data() + (m_size.x * pos.y + pos.x));
	}

	inline Color Image::operator [](const Point pos)&&
	{
		return *(m_pixels.data() + (m_size.x * pos.y + pos.x));
	}

# ifdef __cpp_multidimensional_subscript

	inline Color& Image::operator [](const size_t y, const size_t x)&
	{
		return *(m_pixels.data() + (m_size.x * y + x));
	}

	inline const Color& Image::operator [](const size_t y, const size_t x) const&
	{
		return *(m_pixels.data() + (m_size.x * y + x));
	}

	inline Color Image::operator [](const size_t y, const size_t x)&&
	{
		return *(m_pixels.data() + (m_size.x * y + x));
	}

# endif

	////////////////////////////////////////////////////////////////
	//
	//	data
	//
	////////////////////////////////////////////////////////////////

	inline Color* Image::data()
	{
		return m_pixels.data();
	}

	inline const Color* Image::data() const
	{
		return m_pixels.data();
	}

	////////////////////////////////////////////////////////////////
	//
	//	dataAsUint8
	//
	////////////////////////////////////////////////////////////////

	inline uint8* Image::dataAsUint8()
	{
		return static_cast<uint8*>(static_cast<void*>(m_pixels.data()));
	}

	inline const uint8* Image::dataAsUint8() const
	{
		return static_cast<const uint8*>(static_cast<const void*>(m_pixels.data()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	begin, end
	//
	////////////////////////////////////////////////////////////////

	inline Image::iterator Image::begin() noexcept
	{
		return m_pixels.begin();
	}

	inline Image::iterator Image::end() noexcept
	{
		return m_pixels.end();
	}

	inline Image::const_iterator Image::begin() const noexcept
	{
		return m_pixels.begin();
	}

	inline Image::const_iterator Image::end() const noexcept
	{
		return m_pixels.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin, cend
	//
	////////////////////////////////////////////////////////////////

	inline Image::const_iterator Image::cbegin() const noexcept
	{
		return m_pixels.cbegin();
	}

	inline Image::const_iterator Image::cend() const noexcept
	{
		return m_pixels.cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rbegin, rend
	//
	////////////////////////////////////////////////////////////////

	inline Image::reverse_iterator Image::rbegin() noexcept
	{
		return m_pixels.rbegin();
	}

	inline Image::reverse_iterator Image::rend() noexcept
	{
		return m_pixels.rend();
	}

	inline Image::const_reverse_iterator Image::rbegin() const noexcept
	{
		return m_pixels.rbegin();
	}

	inline Image::const_reverse_iterator Image::rend() const noexcept
	{
		return m_pixels.rend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	crbegin, crend
	//
	////////////////////////////////////////////////////////////////

	inline Image::const_reverse_iterator Image::crbegin() const noexcept
	{
		return m_pixels.crbegin();
	}

	inline Image::const_reverse_iterator Image::crend() const noexcept
	{
		return m_pixels.crend();
	}






	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	constexpr Size Image::ValidImageSizeOrEmpty(const Size size) noexcept
	{
		if (InRange(size.x, 1, Image::MaxWidth)
			&& InRange(size.y, 1, Image::MaxHeight))
		{
			return size;
		}
		else
		{
			return{ 0, 0 };
		}
	}
}

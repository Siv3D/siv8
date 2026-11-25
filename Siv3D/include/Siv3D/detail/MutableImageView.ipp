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

# pragma once
# include <cassert>
# include "../ImageView.hpp"

namespace s3d
{
	inline MutableImageView::MutableImageView(Color* data, const Size size, const int32 stride) noexcept
		: m_data{ data }, m_size{ size }, m_stride{ stride } {}

	inline int32 MutableImageView::width() const noexcept
	{
		return m_size.x;
	}

	inline int32 MutableImageView::height() const noexcept
	{
		return m_size.y;
	}

	inline Size MutableImageView::size() const noexcept
	{
		return m_size;
	}

	inline uint32 MutableImageView::bytesPerRow() const noexcept
	{
		return static_cast<uint32>(m_stride * sizeof(Color));
	}

	inline size_t MutableImageView::pixelCount() const noexcept
	{
		return (static_cast<size_t>(m_size.x) * m_size.y);
	}

	inline bool MutableImageView::isEmpty() const noexcept
	{
		return (m_data == nullptr);
	}

	inline MutableImageView::operator bool() const noexcept
	{
		return (not isEmpty());
	}

	inline Color* MutableImageView::data() const noexcept
	{
		return m_data;
	}

	inline std::span<Color> MutableImageView::row(const size_t y) const noexcept
	{
		assert(y < static_cast<size_t>(m_size.y));
		return { (m_data + y * m_stride), static_cast<size_t>(m_size.x) };
	}

	inline Color* MutableImageView::operator [](const size_t y) const noexcept
	{
		return row(y).data();
	}

	inline Color& MutableImageView::operator [](const Point pos) const noexcept
	{
		return (*this)[static_cast<size_t>(pos.y), static_cast<size_t>(pos.x)];
	}

	inline Color& MutableImageView::operator [](const size_t y, const size_t x) const noexcept
	{
		assert(x < static_cast<size_t>(m_size.x));
		return row(y)[x];
	}

	inline void MutableImageView::overwrite(const MutableImageView dst, const Point pos) const noexcept
	{
		ImageView{ *this }.overwrite(dst, pos);
	}

	inline void MutableImageView::overwrite(Image& dst, const Point pos) const noexcept
	{
		ImageView{ *this }.overwrite(dst, pos);
	}

	inline void MutableImageView::copyTo(Image& dst) const
	{
		ImageView{ *this }.copyTo(dst);
	}
}

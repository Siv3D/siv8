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
# include "../MutableImageView.hpp"

namespace s3d
{
	inline ImageView::ImageView(const MutableImageView view) noexcept
		: m_data{ view.m_data }, m_size{ view.m_size }, m_stride{ view.m_stride } {}

	inline ImageView::ImageView(const Color* data, const Size size, const int32 stride) noexcept
		: m_data{ data }, m_size{ size }, m_stride{ stride } {}

	inline int32 ImageView::width() const noexcept
	{
		return m_size.x;
	}

	inline int32 ImageView::height() const noexcept
	{
		return m_size.y;
	}

	inline Size ImageView::size() const noexcept
	{
		return m_size;
	}

	inline uint32 ImageView::bytesPerRow() const noexcept
	{
		return static_cast<uint32>(m_stride * sizeof(Color));
	}

	inline size_t ImageView::pixelCount() const noexcept
	{
		return (static_cast<size_t>(m_size.x) * m_size.y);
	}

	inline bool ImageView::isEmpty() const noexcept
	{
		return (m_data == nullptr);
	}

	inline ImageView::operator bool() const noexcept
	{
		return (not isEmpty());
	}

	inline const Color* ImageView::data() const noexcept
	{
		return m_data;
	}

	inline std::span<const Color> ImageView::row(const size_t y) const noexcept
	{
		assert(y < static_cast<size_t>(m_size.y));
		return { (m_data + y * m_stride), static_cast<size_t>(m_size.x) };
	}

	inline const Color* ImageView::operator [](const size_t y) const noexcept
	{
		return row(y).data();
	}

	inline const Color& ImageView::operator [](const Point pos) const noexcept
	{
		return (*this)[static_cast<size_t>(pos.y), static_cast<size_t>(pos.x)];
	}

	inline const Color& ImageView::operator [](const size_t y, const size_t x) const noexcept
	{
		assert(x < static_cast<size_t>(m_size.x));
		return row(y)[x];
	}
}

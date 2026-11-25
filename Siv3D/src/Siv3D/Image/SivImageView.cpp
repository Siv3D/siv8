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
# include "ImageRegion.hpp"

namespace s3d
{
	MutableImageView Image::operator ()(const int32 x, const int32 y, const int32 size) & noexcept
	{
		return (*this)(Rect{ x, y, size });
	}

	ImageView Image::operator ()(const int32 x, const int32 y, const int32 size) const& noexcept
	{
		return (*this)(Rect{ x, y, size });
	}

	MutableImageView Image::operator ()(const int32 x, const int32 y, const int32 w, const int32 h) & noexcept
	{
		return (*this)(Rect{ x, y, w, h });
	}

	ImageView Image::operator ()(const int32 x, const int32 y, const int32 w, const int32 h) const& noexcept
	{
		return (*this)(Rect{ x, y, w, h });
	}

	MutableImageView Image::operator ()(const Point pos, const int32 w, const int32 h) & noexcept
	{
		return (*this)(Rect{ pos, w, h });
	}

	ImageView Image::operator ()(const Point pos, const int32 w, const int32 h) const& noexcept
	{
		return (*this)(Rect{ pos, w, h });
	}

	MutableImageView Image::operator ()(const int32 x, const int32 y, const Size size) & noexcept
	{
		return (*this)(Rect{ x, y, size });
	}

	ImageView Image::operator ()(const int32 x, const int32 y, const Size size) const& noexcept
	{
		return (*this)(Rect{ x, y, size });
	}

	MutableImageView Image::operator ()(const Point pos, const int32 size) & noexcept
	{
		return (*this)(Rect{ pos, size });
	}

	ImageView Image::operator ()(const Point pos, const int32 size) const& noexcept
	{
		return (*this)(Rect{ pos, size });
	}

	MutableImageView Image::operator ()(const Point pos, const Size size) & noexcept
	{
		return (*this)(Rect{ pos, size });
	}

	ImageView Image::operator ()(const Point pos, const Size size) const& noexcept
	{
		return (*this)(Rect{ pos, size });
	}

	MutableImageView Image::operator ()(const Rect rect) & noexcept
	{
		return MutableImageView{ m_pixels.data(), m_size, m_size.x }.subview(rect);
	}

	ImageView Image::operator ()(const Rect rect) const& noexcept
	{
		return ImageView{ m_pixels.data(), m_size, m_size.x }.subview(rect);
	}

	ImageView ImageView::subview(const Rect rect) const noexcept
	{
		const Rect region = detail::ImageRegion::Clip(m_size, rect);
		if ((region.w == 0) || (region.h == 0))
		{
			return {};
		}

		return { (m_data + static_cast<size_t>(region.y) * m_stride + region.x), region.size, m_stride };
	}

	Image ImageView::cloned() const
	{
		Image result;
		copyTo(result);
		return result;
	}

	void ImageView::copyTo(Image& dst) const
	{
		if (isEmpty())
		{
			dst.clear();
			return;
		}

		// Test for an alias before resizing dst. Every nonempty view starts at
		// a live pixel in its owner; no external-buffer construction is exposed.
		const auto less = std::less<const Color*>{};
		if (not dst.isEmpty() && not less(m_data, dst.data()) && less(m_data, (dst.data() + dst.pixelCount())))
		{
			if ((m_data == dst.data()) && (m_size == dst.size()))
			{
				return;
			}

			Image result{ m_size };
			detail::ImageRegion::Copy(m_data, m_stride, result.data(), result.width(), m_size);
			dst.swap(result);
			return;
		}

		dst.resize(m_size);
		detail::ImageRegion::Copy(m_data, m_stride, dst.data(), dst.width(), m_size);
	}

	void ImageView::overwrite(const MutableImageView dst, const Point pos) const noexcept
	{
		detail::ImageRegion::Overwrite<true>(m_data, m_size, m_stride,
			dst.m_data, dst.m_size, dst.m_stride, pos);
	}

	void ImageView::overwrite(Image& dst, const Point pos) const noexcept
	{
		detail::ImageRegion::Overwrite<true>(m_data, m_size, m_stride,
			dst.data(), dst.size(), dst.width(), pos);
	}

	MutableImageView MutableImageView::subview(const Rect rect) const noexcept
	{
		const Rect region = detail::ImageRegion::Clip(m_size, rect);
		if ((region.w == 0) || (region.h == 0))
		{
			return {};
		}

		return { (m_data + static_cast<size_t>(region.y) * m_stride + region.x), region.size, m_stride };
	}

	Image MutableImageView::cloned() const
	{
		return ImageView{ *this }.cloned();
	}

	void MutableImageView::fill(const Color color) const noexcept
	{
		if (not isEmpty())
		{
			detail::ImageRegion::Fill(m_data, m_size, m_stride, color);
		}
	}
}

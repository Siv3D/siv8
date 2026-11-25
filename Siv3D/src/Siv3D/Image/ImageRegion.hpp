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
# include <algorithm>
# include <cassert>
# include <cstring>
# include <functional>
# include <Siv3D/Color.hpp>
# include <Siv3D/2DShapes.hpp>

namespace s3d::detail::ImageRegion
{
	// Clip an interval without forming offset + length outside its intersection.
	// The negative-offset case is rejected before negating/subtracting offset.
	[[nodiscard]]
	inline bool ClipAxis(const int32 limit, int32& offset, int32& length) noexcept
	{
		if ((length <= 0) || (limit <= offset) || (offset <= -length))
		{
			return false;
		}

		if (offset < 0)
		{
			length += offset;
			offset = 0;
		}

		length = std::min(length, (limit - offset));
		return (0 < length);
	}

	[[nodiscard]]
	inline Rect Clip(const Size bounds, Rect rect) noexcept
	{
		if (not ClipAxis(bounds.x, rect.x, rect.w)
			|| not ClipAxis(bounds.y, rect.y, rect.h))
		{
			return Rect{ 0, 0, 0, 0 };
		}

		return rect;
	}

	inline void Fill(Color* dst, const size_t count, const Color color) noexcept
	{
		std::fill_n(dst, count, color);
	}

	// Nonempty, validated rectangles only. Strides are measured in Color elements.
	inline void Fill(Color* dst, const Size size, const int32 stride, const Color color) noexcept
	{
		if (size.x == stride)
		{
			Fill(dst, (static_cast<size_t>(size.x) * size.y), color);
			return;
		}

		for (int32 y = 0; y < size.y; ++y)
		{
			Fill((dst + static_cast<size_t>(y) * stride), size.x, color);
		}
	}

	// No overlap: owners known to differ can use this without alias checks.
	inline void Copy(const Color* src, const int32 srcStride, Color* dst, const int32 dstStride, const Size size) noexcept
	{
		const size_t rowBytes = (size.x * sizeof(Color));

		if ((size.x == srcStride) && (size.x == dstStride))
		{
			std::memcpy(dst, src, (rowBytes * size.y));
			return;
		}

		for (int32 y = 0; y < size.y; ++y)
		{
			std::memcpy((dst + static_cast<size_t>(y) * dstStride),
				(src + static_cast<size_t>(y) * srcStride), rowBytes);
		}
	}

	inline void CopyPossiblyOverlapping(const Color* src, const int32 srcStride,
		Color* dst, const int32 dstStride, const Size size) noexcept
	{
		const auto less = std::less<const Color*>{};
		const Color* srcEnd = (src + static_cast<size_t>(size.y - 1) * srcStride + size.x);
		const Color* dstEnd = (dst + static_cast<size_t>(size.y - 1) * dstStride + size.x);
		if (not less(src, dstEnd) || not less(dst, srcEnd))
		{
			Copy(src, srcStride, dst, dstStride, size);
			return;
		}

		// Views can only be obtained from Image, so overlapping views have the
		// same row stride. Directional row moves preserve the original pixels.
		assert(srcStride == dstStride);
		if (src == dst)
		{
			return;
		}

		const size_t rowBytes = (size.x * sizeof(Color));
		if (size.x == srcStride)
		{
			std::memmove(dst, src, (rowBytes * size.y));
		}
		else if (less(src, dst))
		{
			for (int32 y = size.y; y-- > 0;)
			{
				std::memmove((dst + static_cast<size_t>(y) * dstStride),
					(src + static_cast<size_t>(y) * srcStride), rowBytes);
			}
		}
		else
		{
			for (int32 y = 0; y < size.y; ++y)
			{
				std::memmove((dst + static_cast<size_t>(y) * dstStride),
					(src + static_cast<size_t>(y) * srcStride), rowBytes);
			}
		}
	}

	template <bool MayOverlap>
	inline void Overwrite(const Color* src, const Size srcSize, const int32 srcStride,
		Color* dst, const Size dstSize, const int32 dstStride, const Point pos) noexcept
	{
		const Rect write = Clip(dstSize, Rect{ pos, srcSize });
		if ((write.w == 0) || (write.h == 0))
		{
			return;
		}

		src += (static_cast<size_t>(write.y - pos.y) * srcStride + (write.x - pos.x));
		dst += (static_cast<size_t>(write.y) * dstStride + write.x);
		if constexpr (MayOverlap)
		{
			CopyPossiblyOverlapping(src, srcStride, dst, dstStride, write.size);
		}
		else
		{
			Copy(src, srcStride, dst, dstStride, write.size);
		}
	}
}

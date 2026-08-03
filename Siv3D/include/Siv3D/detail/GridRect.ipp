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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	bounds
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Rect Grid<Type, Allocator>::bounds() const noexcept
	{
		Rect result;
		result.pos = Point{ 0, 0 };
		result.size = m_size;
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator>& Grid<Type, Allocator>::fill(const Rect rect, const value_type& value)
	{
		return fill(rect.pos, rect.size, value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	subgrid
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class Allocator>
	constexpr Grid<Type, Allocator> Grid<Type, Allocator>::subgrid(const Rect rect) const
	{
		return subgrid(rect.pos, rect.size);
	}
}

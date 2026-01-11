//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	constexpr Subdivision2D::Vertex::Vertex(const Vec2& _pt, const bool _isvirtual, const EdgeID _firstEdge) noexcept
		: firstEdge{ _firstEdge }
		, type{ _isvirtual }
		, pt{ _pt } {}

	constexpr bool Subdivision2D::Vertex::isVirtual() const noexcept
	{
		return (0 < type);
	}

	constexpr bool Subdivision2D::Vertex::isFree() const noexcept
	{
		return (type < 0);
	}

	constexpr Subdivision2D::QuadEdge::QuadEdge(const EdgeID edgeidx)
	{
		assert((edgeidx & 3) == 0);
		next[0] = edgeidx;
		next[1] = (edgeidx + 3);
		next[2] = (edgeidx + 2);
		next[3] = (edgeidx + 1);
	}

	constexpr bool Subdivision2D::QuadEdge::isFree() const noexcept
	{
		return (next[0] <= 0);
	}

	constexpr bool Subdivision2D::Internal::isEmpty() const noexcept
	{
		return (addedPoints == 0);
	}

	constexpr RectF Subdivision2D::Internal::rect() const noexcept
	{
		return{ topLeft, (bottomRight - topLeft) };
	}
}

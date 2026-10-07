//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once

namespace s3d
{
	inline size_t Spline2D::segmentCount() const noexcept
	{
		return m_segments.size();
	}

	inline bool Spline2D::isEmpty() const noexcept
	{
		return m_segments.isEmpty();
	}

	inline bool Spline2D::isClosed() const noexcept
	{
		return (m_closed && not isEmpty());
	}

	inline std::span<const Bezier3> Spline2D::segments() const& noexcept
	{
		return { m_segments.data(), m_segments.size() };
	}

	inline void Spline2D::clear() noexcept
	{
		m_segments.clear();
		m_closed = false;
	}

	inline void Spline2D::swap(Spline2D& other) noexcept
	{
		m_segments.swap(other.m_segments);
		std::swap(m_closed, other.m_closed);
	}
}

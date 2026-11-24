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

	inline const Spline2D& Spline2D::paint(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing, const double targetError, const int32 maxSegmentsPerCurve) const
	{
		return paint(dst, LineCap::Round, LineCap::Round, 1.0, color, enableAntialiasing, targetError, maxSegmentsPerCurve);
	}

	inline const Spline2D& Spline2D::paint(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing, const double targetError, const int32 maxSegmentsPerCurve) const
	{
		return paint(dst, LineCap::Round, LineCap::Round, thickness, color, enableAntialiasing, targetError, maxSegmentsPerCurve);
	}

	inline const Spline2D& Spline2D::paint(Image& dst, const LineCap lineCap, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing, const double targetError, const int32 maxSegmentsPerCurve) const
	{
		return paint(dst, lineCap, lineCap, thickness, color, enableAntialiasing, targetError, maxSegmentsPerCurve);
	}

	inline const Spline2D& Spline2D::overwrite(Image& dst, const Color& color, const EnableAntialiasing enableAntialiasing, const double targetError, const int32 maxSegmentsPerCurve) const
	{
		return overwrite(dst, LineCap::Round, LineCap::Round, 1.0, color, enableAntialiasing, targetError, maxSegmentsPerCurve);
	}

	inline const Spline2D& Spline2D::overwrite(Image& dst, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing, const double targetError, const int32 maxSegmentsPerCurve) const
	{
		return overwrite(dst, LineCap::Round, LineCap::Round, thickness, color, enableAntialiasing, targetError, maxSegmentsPerCurve);
	}

	inline const Spline2D& Spline2D::overwrite(Image& dst, const LineCap lineCap, const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing, const double targetError, const int32 maxSegmentsPerCurve) const
	{
		return overwrite(dst, lineCap, lineCap, thickness, color, enableAntialiasing, targetError, maxSegmentsPerCurve);
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

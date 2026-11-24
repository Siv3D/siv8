//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <algorithm>
# include <stdexcept>
# include <Siv3D/Spline2DMeasure.hpp>
# include <Siv3D/Bezier3/Bezier3ArcLength.hpp>

namespace s3d
{
	Spline2DMeasure::Spline2DMeasure(Spline2D spline)
		: m_spline{ std::move(spline) }
	{
		m_tables.reserve(m_spline.segmentCount());
		m_prefixLengths.reserve(m_spline.segmentCount() + 1);
		m_prefixLengths.push_back(0.0);
		for (const Bezier3& curve : m_spline.segments())
		{
			m_tables.push_back(detail::BuildBezier3ArcLengthTable(curve));
			m_prefixLengths.push_back(m_prefixLengths.back() + m_tables.back().length());
		}
	}

	SplineLocation Spline2DMeasure::locationAtDistance(double distanceFromStart, const DistanceMode mode) const
	{
		if (isEmpty())
		{
			throw std::out_of_range{ "Spline2DMeasure::locationAtDistance(): curve is empty" };
		}
		const double total = length();
		if (total == 0.0)
		{
			return { 0, 0.0 };
		}
		if (mode == DistanceMode::Wrap)
		{
			distanceFromStart = std::fmod(distanceFromStart, total);
			if (distanceFromStart < 0.0)
			{
				distanceFromStart += total;
			}
		}
		if (distanceFromStart <= 0.0)
		{
			return { 0, 0.0 };
		}
		if (total <= distanceFromStart)
		{
			return { (m_spline.segmentCount() - 1), 1.0 };
		}
		const size_t index = (static_cast<size_t>(std::upper_bound(m_prefixLengths.begin(), m_prefixLengths.end(), distanceFromStart) - m_prefixLengths.begin()) - 1);
		const double localDistance = (distanceFromStart - m_prefixLengths[index]);
		const Bezier3& curve = m_spline.segments()[index];
		return { index, detail::Bezier3TAtDistance(curve, m_tables[index], localDistance) };
	}

	Vec2 Spline2DMeasure::pointAtDistance(const double distanceFromStart, const DistanceMode mode) const
	{
		const SplineLocation location = locationAtDistance(distanceFromStart, mode);
		return m_spline.segments()[location.segment].pointAt(location.t);
	}

	double Spline2DMeasure::distanceAt(const SplineLocation location) const
	{
		const Bezier3& curve = m_spline.segment(location.segment);
		return (m_prefixLengths[location.segment] + detail::Bezier3DistanceAtT(curve, m_tables[location.segment], location.t));
	}

	void Spline2DMeasure::swap(Spline2DMeasure& other) noexcept
	{
		m_spline.swap(other.m_spline);
		m_tables.swap(other.m_tables);
		m_prefixLengths.swap(other.m_prefixLengths);
	}
}

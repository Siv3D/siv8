//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <algorithm>
# include <stdexcept>
# include <Siv3D/Spline2D.hpp>
# include <Siv3D/Geometry2D/BezierGeometry.hpp>
# include <Siv3D/Geometry2D/BezierTessellation.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		void ValidateTessellation(const double targetError, const int32 maxSegmentsPerCurve)
		{
			if ((targetError <= 0.0) || (maxSegmentsPerCurve < 1) || (4096 < maxSegmentsPerCurve))
			{
				throw std::invalid_argument{ "Spline2D: positive targetError and maxSegmentsPerCurve in [1, 4096] required" };
			}
		}
	}

	Spline2D Spline2D::FromCatmullRom(const std::span<const Vec2> points, const CloseRing closeRing,
		const CatmullRomParameterization parameterization)
	{
		Array<Vec2> knots;
		knots.reserve(points.size());
		for (const Vec2 point : points)
		{
			if (knots.isEmpty() || (knots.back() != point))
			{
				knots.push_back(point);
			}
		}
		if (closeRing && (knots.size() > 1) && (knots.front() == knots.back()))
		{
			knots.pop_back();
		}
		const size_t count = knots.size();
		if (closeRing && (count < 3))
		{
			throw std::invalid_argument{ "Spline2D::FromCatmullRom(): a closed curve needs at least three knots" };
		}
		Spline2D result;
		if (count < 2)
		{
			return result;
		}
		result.m_closed = static_cast<bool>(closeRing);
		const size_t segmentCount = (closeRing ? count : (count - 1));
		Array<double> intervals(segmentCount, 1.0);
		if (parameterization == CatmullRomParameterization::Centripetal)
		{
			for (size_t i = 0; i < segmentCount; ++i)
			{
				intervals[i] = std::sqrt(knots[i].distanceFrom(knots[(i + 1) % count]));
			}
		}
		const auto Tangent = [&](const size_t i) -> Vec2
		{
			if (not closeRing)
			{
				if (i == 0)
				{
					return ((knots[1] - knots[0]) / intervals.front());
				}
				if ((i + 1) == count)
				{
					return ((knots[i] - knots[i - 1]) / intervals.back());
				}
			}
			const size_t previous = ((i == 0) ? (count - 1) : (i - 1));
			const size_t next = ((i + 1) % count);
			const double before = intervals[previous];
			const double after = intervals[i];
			return (((knots[i] - knots[previous]) * (after / before)
				+ (knots[next] - knots[i]) * (before / after)) / (before + after));
		};
		result.m_segments.reserve(segmentCount);
		Vec2 startTangent = Tangent(0);
		for (size_t i = 0; i < segmentCount; ++i)
		{
			const size_t next = ((i + 1) % count);
			const Vec2 endTangent = Tangent(next);
			result.m_segments.push_back(Bezier3::FromHermite(knots[i], (startTangent * intervals[i]),
				knots[next], (endTangent * intervals[i])));
			startTangent = endTangent;
		}
		return result;
	}

	Spline2D Spline2D::FromBezierSegments(const std::span<const Bezier3> segments, const CloseRing closeRing)
	{
		for (size_t i = 1; i < segments.size(); ++i)
		{
			if (segments[i - 1].p3 != segments[i].p0)
			{
				throw std::invalid_argument{ "Spline2D::FromBezierSegments(): adjacent endpoints must match" };
			}
		}
		if (closeRing && not segments.empty() && (segments.back().p3 != segments.front().p0))
		{
			throw std::invalid_argument{ "Spline2D::FromBezierSegments(): the closing endpoints must match" };
		}
		Spline2D result;
		result.m_segments.assign(segments.begin(), segments.end());
		result.m_closed = (closeRing && not segments.empty());
		return result;
	}

	const Bezier3& Spline2D::segment(const size_t index) const&
	{
		if (m_segments.size() <= index)
		{
			throw std::out_of_range{ "Spline2D::segment(): index out of range" };
		}
		return m_segments[index];
	}

	Vec2 Spline2D::pointAt(const SplineLocation location) const
	{
		return segment(location.segment).pointAt(location.t);
	}

	Optional<SplineClosestPoint2D> Spline2D::computeClosestPoint(const Vec2 point) const
	{
		Optional<SplineClosestPoint2D> best;
		for (size_t i = 0; i < m_segments.size(); ++i)
		{
			const Bezier3& curve = m_segments[i];
			if (best)
			{
				const RectF bounds = curve.controlPointsBoundingRect();
				const Vec2 clamped{ Clamp(point.x, bounds.x, (bounds.x + bounds.w)), Clamp(point.y, bounds.y, (bounds.y + bounds.h)) };
				if (best->distanceSq < point.distanceFromSq(clamped))
				{
					continue;
				}
			}
			const auto candidate = detail::ClosestPointOnBezier(curve, point);
			if (not best || (candidate.distanceSq < best->distanceSq))
			{
				best = SplineClosestPoint2D{ { i, candidate.parameter }, candidate.point, candidate.distanceSq };
			}
		}
		return best;
	}

	Optional<RectF> Spline2D::computeBoundingRect() const
	{
		if (isEmpty())
		{
			return none;
		}
		RectF result = m_segments.front().computeBoundingRect();
		for (size_t i = 1; i < m_segments.size(); ++i)
		{
			const RectF bounds = m_segments[i].computeBoundingRect();
			const double right = Max((result.x + result.w), (bounds.x + bounds.w));
			const double bottom = Max((result.y + result.h), (bounds.y + bounds.h));
			result.x = Min(result.x, bounds.x);
			result.y = Min(result.y, bounds.y);
			result.w = (right - result.x);
			result.h = (bottom - result.y);
		}
		return result;
	}

	LineString Spline2D::toLineString(const int32 subdivisionsPerSegment) const
	{
		LineString result;
		toLineString(result, subdivisionsPerSegment);
		return result;
	}

	void Spline2D::toLineString(LineString& destination, int32 subdivisionsPerSegment) const
	{
		destination.clear();
		if (isEmpty())
		{
			return;
		}
		subdivisionsPerSegment = Max(1, subdivisionsPerSegment);
		const size_t subdivisions = static_cast<size_t>(subdivisionsPerSegment);
		if (m_segments.size() > ((destination.max_size() - 1) / subdivisions))
		{
			throw std::length_error{ "Spline2D::toLineString(): too many vertices" };
		}
		destination.reserve(m_segments.size() * subdivisions + (not m_closed));
		for (const Bezier3& curve : m_segments)
		{
			destination.push_back(curve.p0);
			for (int32 i = 1; i < subdivisionsPerSegment; ++i)
			{
				destination.push_back(curve.pointAt(static_cast<double>(i) / subdivisionsPerSegment));
			}
		}
		if (not m_closed)
		{
			destination.push_back(m_segments.back().p3);
		}
	}

	LineString Spline2D::toLineStringAdaptive(const double targetError, const int32 maxSegmentsPerCurve) const
	{
		LineString result;
		toLineStringAdaptive(result, targetError, maxSegmentsPerCurve);
		return result;
	}

	void Spline2D::toLineStringAdaptive(LineString& destination, const double targetError, const int32 maxSegmentsPerCurve) const
	{
		ValidateTessellation(targetError, maxSegmentsPerCurve);
		detail::BezierTessellation::Workspace workspace;
		detail::BezierTessellation::FlattenPath(destination, workspace, m_segments, m_closed, targetError,
			static_cast<uint32>(maxSegmentsPerCurve), [](const auto& source) -> const auto& { return source; });
	}

	const Spline2D& Spline2D::paint(Image& dst, const LineCap startCap, const LineCap endCap,
		const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing,
		const double targetError, const int32 maxSegmentsPerCurve) const
	{
		const LineString outline = toLineStringAdaptive(targetError, maxSegmentsPerCurve);
		if (isClosed())
		{
			outline.paintClosed(dst, thickness, color, enableAntialiasing);
		}
		else
		{
			outline.paint(dst, startCap, endCap, thickness, color, enableAntialiasing);
		}
		return *this;
	}

	const Spline2D& Spline2D::overwrite(Image& dst, const LineCap startCap, const LineCap endCap,
		const double thickness, const Color& color, const EnableAntialiasing enableAntialiasing,
		const double targetError, const int32 maxSegmentsPerCurve) const
	{
		const LineString outline = toLineStringAdaptive(targetError, maxSegmentsPerCurve);
		if (isClosed())
		{
			outline.overwriteClosed(dst, thickness, color, enableAntialiasing);
		}
		else
		{
			outline.overwrite(dst, startCap, endCap, thickness, color, enableAntialiasing);
		}
		return *this;
	}

	const Spline2D& Spline2D::draw(const double thickness, const ColorF& color, const double targetError, const int32 maxSegmentsPerCurve) const
	{
		ValidateTessellation(targetError, maxSegmentsPerCurve);
		SIV3D_ENGINE(Renderer2D)->addSpline(m_segments, isClosed() ? CloseRing::Yes : CloseRing::No,
			static_cast<float>(thickness), color.toFloat4(), targetError, maxSegmentsPerCurve);
		return *this;
	}

	const Spline2D& Spline2D::draw(const LineStyle& style, const double thickness, const ColorF& color, const double targetError, const int32 maxSegmentsPerCurve) const
	{
		ValidateTessellation(targetError, maxSegmentsPerCurve);
		if (thickness == 0.0) return *this;
		SIV3D_ENGINE(Renderer2D)->addSplineStyled(m_segments, isClosed() ? CloseRing::Yes : CloseRing::No,
			static_cast<float>(thickness), style, color.toFloat4(), targetError, maxSegmentsPerCurve);
		return *this;
	}

	const Spline2D& Spline2D::drawArrow(const double thickness, const SizeF& headSize, const ColorF& color,
		const double targetError, const int32 maxSegmentsPerCurve) const
	{
		return drawArrow(LineCap::Flat, thickness, headSize, color, targetError, maxSegmentsPerCurve);
	}

	const Spline2D& Spline2D::drawArrow(const LineCap startCap, const double thickness, const SizeF& headSize, const ColorF& color,
		const double targetError, const int32 maxSegmentsPerCurve) const
	{
		ValidateTessellation(targetError, maxSegmentsPerCurve);
		if ((thickness <= 0.0) || (headSize.x <= 0.0) || (headSize.y <= 0.0)) return *this;
		SIV3D_ENGINE(Renderer2D)->addSplineArrow(m_segments, startCap, static_cast<float>(thickness), headSize,
			color.toFloat4(), targetError, maxSegmentsPerCurve);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawDoubleHeadedArrow
	//
	////////////////////////////////////////////////////////////////

	const Spline2D& Spline2D::drawDoubleHeadedArrow(const double thickness, const SizeF& headSize, const ColorF& color, const double targetError, const int32 maxSegmentsPerCurve) const
	{
		ValidateTessellation(targetError, maxSegmentsPerCurve);
		if ((thickness <= 0.0) || (headSize.x <= 0.0) || (headSize.y <= 0.0)) return *this;
		SIV3D_ENGINE(Renderer2D)->addSplineDoubleHeadedArrow(m_segments, static_cast<float>(thickness), headSize, color.toFloat4(), targetError, maxSegmentsPerCurve);
		return *this;
	}

	Spline2D& Spline2D::reverse() noexcept
	{
		std::reverse(m_segments.begin(), m_segments.end());
		for (Bezier3& curve : m_segments)
		{
			curve.reverse();
		}
		return *this;
	}

	Spline2D Spline2D::reversed() const
	{
		Spline2D result{ *this };
		result.reverse();
		return result;
	}
}

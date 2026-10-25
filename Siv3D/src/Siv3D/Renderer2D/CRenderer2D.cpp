//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <algorithm>
# include <cmath>
# include "IRenderer2D.hpp"
# include "SuperEllipseTessellation.hpp"
# include <Siv3D/LineCap.hpp>

namespace s3d::detail::SuperEllipseTessellation
{
	namespace
	{
		constexpr uint32 MaxDepth = 16;

		// On a quadrant, u^n + v^n = 1 is convex or concave. Its unique
		// maximum chord deviation has gradient perpendicular to the chord:
		// u/v = (dv / -du)^(1/(n-1)). Work with ratios <= 1.
		Vec2 FurthestPoint(const Vec2 from, const Vec2 to, const double n,
			const double inverseN, const double inverseGradientExponent)
		{
			const double dx = (from.x - to.x), dy = (to.y - from.y);
			if (n == 2.0)
			{
				return Vec2{ dy, dx } / std::sqrt(dx * dx + dy * dy);
			}
			if (n == 0.5)
			{
				const double inverseSum = (1.0 / (dx + dy));
				const Vec2 root{ dx * inverseSum, dy * inverseSum };
				return (root * root);
			}
			const double ratio = (std::min(dx, dy) / std::max(dx, dy));
			const double power = std::pow(ratio, ((1.0 < n) ? inverseGradientExponent : (n * inverseGradientExponent)));
			const double smaller = ((1.0 < n) ? power : (power * ratio));
			const double sum = (1.0 + ((1.0 < n) ? (power * ratio) : power));
			const double larger = ((n == 4.0) ? (1.0 / std::sqrt(std::sqrt(sum))) : std::pow(sum, -inverseN));
			return ((dy >= dx) == (1.0 < n))
				? Vec2{ larger, smaller * larger } : Vec2{ smaller * larger, larger };
		}
	}

	void Tessellate(Quarter& output, const double a, const double b, const double n,
		const double scale, const double strokeMargin)
	{
		output.limited = false;
		if (n == 1.0)
		{
			output.points[0] = { a, 0 };
			output.points[1] = { 0, b };
			output.count = 2;
			return;
		}

		const double inverseN = (1.0 / n);
		const double inverseGradientExponent = (1.0 / std::abs(n - 1.0));
		const double diagonal = std::pow(0.5, inverseN);
		const double metricA = ((a + strokeMargin) * scale), metricB = ((b + strokeMargin) * scale);
		const double aa = (metricA * metricA), bb = (metricB * metricB);
		struct Segment
		{
			uint16 from, to;
			uint32 depth;
			Vec2 middle;
			double errorSq;
		};
		std::array<Segment, MaxOctantSegments> pending;
		size_t pendingCount = 0;
		output.points[0] = { 1, 0 };
		output.points[1] = { diagonal, diagonal };
		output.count = 2;
		const auto lessError = [](const Segment& left, const Segment& right) { return (left.errorSq < right.errorSq); };
		const auto enqueue = [&](const uint16 from, const uint16 to, const uint32 depth)
		{
			const Vec2 middle = FurthestPoint(output.points[from], output.points[to], n, inverseN, inverseGradientExponent);
			const Vec2 d = (output.points[to] - output.points[from]), m = (middle - output.points[from]);
			const double cross = (d.cross(m) * metricA * metricB);
			// Both this octant and its diagonal reflection must meet the target.
			const double lengthSq = std::min((d.x * d.x * aa + d.y * d.y * bb), (d.x * d.x * bb + d.y * d.y * aa));
			if (not ((cross * cross) > (TargetError * TargetError * lengthSq))) return;
			if (depth == MaxDepth)
			{
				output.limited = true;
				return;
			}
			pending[pendingCount++] = { from, to, depth, middle, (cross * cross) / lengthSq };
			std::push_heap(pending.begin(), pending.begin() + pendingCount, lessError);
		};
		enqueue(0, 1, 0);

		// Refine the worst remaining interval, not the next one in contour order.
		// A fixed heap bounds work and storage even when the target cannot be met.
		while (pendingCount && (output.count <= MaxOctantSegments))
		{
			std::pop_heap(pending.begin(), pending.begin() + pendingCount, lessError);
			const Segment segment = pending[--pendingCount];
			const auto middle = static_cast<uint16>(output.count);
			output.points[output.count++] = segment.middle;
			enqueue(segment.from, middle, segment.depth + 1);
			enqueue(middle, segment.to, segment.depth + 1);
		}
		output.limited |= (pendingCount != 0);
		std::sort(output.points.begin(), output.points.begin() + output.count, [](const Vec2 left, const Vec2 right)
		{
			return ((left.x > right.x) || ((left.x == right.x) && (left.y < right.y)));
		});

		// Reflect the normalized octant before applying the source axes.
		const size_t octantCount = output.count;
		for (size_t i = (octantCount - 1); i-- > 0;)
		{
			const Vec2 p = output.points[i];
			output.points[output.count++] = { p.y, p.x };
		}
		for (size_t i = 0; i < output.count; ++i)
		{
			output.points[i] *= Vec2{ a, b };
		}
	}

	void WriteContour(Array<Vec2>& destination, const Quarter& quarter, const Vec2 center)
	{
		destination.resize((quarter.count - 1) * 4);
		Vec2* out = destination.data();
		VisitContour(quarter, [&](const Vec2 point) { *out++ = (center + point); });
	}
}

namespace s3d
{
	namespace
	{
		template <class Fill>
		void DrawFrame(ISiv3DRenderer2D& renderer, Array<Vec2>& points, const SuperEllipse& shape,
			const float thickness, const Fill& fill)
		{
			detail::SuperEllipseTessellation::Quarter quarter;
			detail::SuperEllipseTessellation::Tessellate(quarter, Abs(shape.a), Abs(shape.b), shape.n,
				renderer.getRasterScaling(), (thickness * 0.5));
			detail::SuperEllipseTessellation::WriteContour(points, quarter, shape.center);
			renderer.addLineString(LineCap::Square, LineCap::Square, points, none, thickness, false, CloseRing::Yes, fill);
		}
	}

	void ISiv3DRenderer2D::addSuperEllipseFrame(const SuperEllipse& shape, const float thickness, const Float4& color)
	{
		DrawFrame(*this, m_curvePoints, shape, thickness, color);
	}

	void ISiv3DRenderer2D::addSuperEllipseFrame(const SuperEllipse& shape, const float thickness, const PatternParameters& pattern)
	{
		DrawFrame(*this, m_curvePoints, shape, thickness, pattern);
	}
}

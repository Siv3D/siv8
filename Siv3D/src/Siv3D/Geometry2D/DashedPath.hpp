//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <algorithm>
# include <array>
# include <cmath>
# include <span>
# include <Siv3D/Array.hpp>
# include <Siv3D/PointVector.hpp>
# include "DashPattern.hpp"

namespace s3d::detail::DashedPath
{
	inline constexpr size_t MaxRuns = DashPattern::MaxRuns;

	struct Node
	{
		Vec2 point;
		double distance;
	};

	struct Run
	{
		size_t offset, count;
		bool closed = false;
	};

	struct Dot
	{
		Float2 start, end;
		Float2 uRange;
	};

	struct Workspace
	{
		Array<Node> path;
		Array<Vec2> points;
		Array<Run> runs;
		Array<Dot> dots;

		std::span<const Vec2> view(const Run& run) const
		{
			return { (points.data() + run.offset), run.count };
		}
	};

	// Cut the authored centerline before the stroke builder adds join points.
	// Fixed intervals follow the existing Line shaders' half-coverage boundaries.
	// Storage is reused; exceeding the budget produces no partial output.
	inline bool Split(Workspace& out, const std::span<const Vec2> source, const bool closed,
		const double width, const LineStyle& style)
	{
		out.path.clear();
		out.points.clear();
		out.runs.clear();
		out.dots.clear();
		if (width <= 0.0) return true;
		const auto append = [&](const Vec2 point)
		{
			if (out.path.empty()) out.path.push_back({ point, 0.0 });
			else if (out.path.back().point != point)
			{
				const auto& previous = out.path.back();
				out.path.push_back({ point, (previous.distance + previous.point.distanceFrom(point)) });
			}
		};
		for (const Vec2 point : source) append(point);
		if (out.path.size() < 2) return true;
		if (closed) append(out.path.front().point);
		const double length = out.path.back().distance;
		DashPattern::Cursor pattern;
		if (not pattern.prepare(length, closed, width, style)) return false;
		const double unit = pattern.unit;
		size_t segment = 0;
		const auto pointAt = [&](const double distance)
		{
			while (((segment + 2) < out.path.size()) && (out.path[segment + 1].distance <= distance)) ++segment;
			const Node& a = out.path[segment];
			const Node& b = out.path[segment + 1];
			return a.point.lerp(b.point, ((distance - a.distance) / (b.distance - a.distance)));
		};
		bool beginsAtSeam = false, endsAtSeam = false;
		for (size_t i = 0; i < pattern.count; ++i)
		{
			const auto [a, b, start] = pattern.next();
			if (b <= a) continue;
			if (pattern.round)
			{
				const double centerDistance = (start + unit * 0.5);
				const Vec2 center = pointAt(centerDistance);
				const auto& p = out.path[segment];
				const auto& q = out.path[segment + 1];
				const Vec2 direction = ((q.point - p.point) / (q.distance - p.distance));
				out.dots.push_back({ Float2{ center + direction * (a - centerDistance) },
					Float2{ center + direction * (b - centerDistance) },
					{ static_cast<float>((a - start) / unit), static_cast<float>((b - start) / unit) } });
				continue;
			}
			const size_t begin = out.points.size();
			out.points.push_back(pointAt(a));
			while (((segment + 2) < out.path.size()) && (out.path[segment + 1].distance < b))
			{
				out.points.push_back(out.path[++segment].point);
			}
			out.points.push_back(pointAt(b));
			out.runs.push_back({ begin, (out.points.size() - begin), false });
			beginsAtSeam |= (a == 0.0);
			endsAtSeam = (b == length);
		}
		if (closed && beginsAtSeam && endsAtSeam)
		{
			Run& front = out.runs.front();
			if (out.runs.size() == 1)
			{
				front.closed = true;
				--front.count;
				out.points.pop_back();
			}
			else
			{
				// Append the beginning after the last run, skipping the shared seam.
				const Run back = out.runs.back();
				const size_t frontCount = front.count;
				out.points.reserve(out.points.size() + frontCount - 1);
				for (size_t i = 1; i < frontCount; ++i) out.points.push_back(out.points[i]);
				front = { back.offset, (back.count + frontCount - 1), false };
				out.runs.pop_back();
			}
		}
		return true;
	}
}

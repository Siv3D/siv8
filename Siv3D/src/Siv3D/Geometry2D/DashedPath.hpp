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
# include <Siv3D/LineStyle.hpp>

namespace s3d::detail::DashedPath
{
	inline constexpr size_t MaxRuns = 4096;

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
		const double extension = ((not closed && style.cap != LineCap::Flat) ? (width * 0.5) : 0.0);
		const double domainStart = -extension, domainEnd = (length + extension);
		const bool round = (style.type == LinePattern::RoundDot);
		const double unit = ((round && extension) ? ((length + width) / (std::ceil(length / width * 0.5) * 2 + 1)) : width);
		struct Series
		{
			double first = 0, size = 0;
			size_t count = 0, index = 0;
		};
		std::array<Series, 2> series{};
		double period = 2;
		size_t seriesCount = 1;
		switch (style.type)
		{
		case LinePattern::Dotted:
		case LinePattern::RoundDot: series[0] = { 0, 1 }; break;
		case LinePattern::Dashed: period = 4; series[0] = { -0.2, 2.4 }; break;
		case LinePattern::LongDash: period = 10; series[0] = { -2.5, 7 }; break;
		case LinePattern::DashDot:
			period = 10; series[0] = { -2, 6 }; series[1] = { 5.5, 1 }; seriesCount = 2; break;
		default: return true; // Solid is submitted directly by the caller.
		}
		const double phase = std::fmod(style.dotOffset, period), pitch = (period * unit);
		size_t count = 0;
		for (size_t i = 0; i < seriesCount; ++i)
		{
			auto& s = series[i];
			s.first = ((s.first + phase) * unit - extension);
			s.size *= unit;
			double n;
			if (round && closed)
			{
				// Keep whole dots with centers in [0, length), avoiding a split dot at the seam.
				s.first += std::ceil(-(s.first + unit * 0.5) / pitch) * pitch;
				n = std::max(0.0, std::ceil((length - s.first - unit * 0.5) / pitch));
			}
			else
			{
				s.first += (std::floor((domainStart - s.first - s.size) / pitch) + 1) * pitch;
				n = std::max(0.0, std::ceil((domainEnd - s.first) / pitch));
			}
			if ((MaxRuns - count) < n) return false;
			s.count = static_cast<size_t>(n);
			count += s.count;
		}
		size_t segment = 0;
		const auto pointAt = [&](const double distance)
		{
			while (((segment + 2) < out.path.size()) && (out.path[segment + 1].distance <= distance)) ++segment;
			const Node& a = out.path[segment];
			const Node& b = out.path[segment + 1];
			return a.point.lerp(b.point, ((distance - a.distance) / (b.distance - a.distance)));
		};
		bool beginsAtSeam = false, endsAtSeam = false;
		for (size_t i = 0; i < count; ++i)
		{
			size_t next = 0;
			if ((series[0].index == series[0].count) || ((seriesCount == 2)
				&& (series[1].index < series[1].count)
				&& ((series[1].first + series[1].index * pitch) < (series[0].first + series[0].index * pitch)))) next = 1;
			auto& s = series[next];
			// Multiplication avoids cumulative phase drift and bounds the loop.
			const double start = (s.first + s.index++ * pitch);
			const double a = ((round && closed) ? start : std::max(domainStart, start));
			const double b = ((round && closed) ? (start + s.size) : std::min(domainEnd, (start + s.size)));
			if (b <= a) continue;
			if (round)
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

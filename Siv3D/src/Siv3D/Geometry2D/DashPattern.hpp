//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <array>
# include <algorithm>
# include <cmath>
# include <Siv3D/LineStyle.hpp>

namespace s3d::detail::DashPattern
{
	inline constexpr size_t MaxRuns = 4096;

	struct Interval
	{
		double start, end, origin;
	};

	// Distance-only pattern placement shared by sampled paths and analytic arcs.
	struct Cursor
	{
		struct Series
		{
			double first = 0, size = 0;
			size_t count = 0, index = 0;
		};
		std::array<Series, 2> series{};
		double extension = 0, domainStart = 0, domainEnd = 0, unit = 0, pitch = 0, cycles = 0;
		size_t count = 0, seriesCount = 1;
		bool round = false, closed = false;

		SIV3D_ALWAYS_INLINE
		bool prepare(const double length, const bool isClosed, const double width, const LineStyle& style)
		{
			return prepareImpl<false>(length, isClosed, width, style, 0);
		}

		SIV3D_ALWAYS_INLINE
		bool prepareArc(const double length, const double perimeter, const double width, const LineStyle& style)
		{
			return prepareImpl<true>(length, false, width, style, perimeter);
		}

		template <bool Arc>
		SIV3D_ALWAYS_INLINE
		bool prepareImpl(const double length, const bool isClosed, const double width, const LineStyle& style, const double perimeter)
		{
			*this = {};
			closed = isClosed;
			if ((length <= 0.0) || (width <= 0.0)) return true;
			extension = ((not closed && style.cap != LineCap::Flat) ? (width * 0.5) : 0.0);
			if constexpr (Arc) extension = std::min(extension, std::max(0.0, (perimeter - length) * 0.5));
			domainStart = -extension; domainEnd = (length + extension);
			round = (style.type == LinePattern::RoundDot);
			unit = ((not Arc && round && extension) ? ((length + width) / (std::ceil(length / width * 0.5) * 2 + 1)) : width);
			double period = 2;
			seriesCount = 1;
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
			const double phase = std::fmod(style.dotOffset, period);
			if constexpr (Arc)
			{
				cycles = std::max(1.0, std::round(perimeter / (period * width)));
				unit = (perimeter / (cycles * period));
			}
			pitch = (period * unit);
			const double fittedUnit = unit;
			// A dot keeps its circular diameter. Its clipping interval is bounded
			// by one pitch, including a single dot on a very small circumference.
			if constexpr (Arc) if (round) unit = std::min(width, pitch);
			count = 0;
			for (size_t i = 0; i < seriesCount; ++i)
			{
				auto& s = series[i];
				if constexpr (Arc)
					s.first = (round ? ((0.5 + phase) * fittedUnit - unit * 0.5) : ((s.first + phase) * unit));
				else s.first = ((s.first + phase) * unit - extension);
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
				const size_t seamFragment = ((Arc && round && n > cycles) ? 1 : 0);
				if ((MaxRuns + seamFragment - count) < n) return false;
				s.count = static_cast<size_t>(n);
				count += s.count;
			}
			return true;
		}

		SIV3D_ALWAYS_INLINE
		Interval next()
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
			return { a, b, start };
		}
	};
}

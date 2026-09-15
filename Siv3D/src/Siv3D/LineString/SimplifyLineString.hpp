//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# pragma once
# include <span>
# include <Siv3D/Array.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/PredefinedYesNo.hpp>

namespace s3d::detail
{
	// Input must not alias result. Both output and scratch storage are reusable.
	inline void SimplifyLineString(const std::span<const Vec2> points,
		const double maxDistance, const CloseRing closeRing,
		Array<Vec2>& result, Array<size_t>& pendingEnds)
	{
		result.clear();
		pendingEnds.clear();
		const size_t n = points.size();
		if ((n < 2) || (maxDistance < 0.0))
		{
			result.assign(points.begin(), points.end());
			return;
		}

		const bool appendClosingPoint = (closeRing && (points.front() != points.back()));
		const size_t last = (appendClosingPoint ? n : (n - 1));
		const double maxDistanceSq = (maxDistance * maxDistance);
		result.push_back(points[0]);

		// Defer right-hand intervals and finish left-hand intervals first, so
		// retained vertices can be emitted in order without a per-vertex flag array.
		size_t begin = 0;
		size_t end = last;
		for (;;)
		{
			// Index n is the implicit closing point; the input needs no copy.
			const Vec2& endPoint = points[(end == n) ? 0 : end];
			const Vec2 direction = (endPoint - points[begin]);
			const double lengthSq = direction.lengthSq();
			double farthestDistanceSq = maxDistanceSq;
			size_t split = begin;

			for (size_t i = (begin + 1); i < end; ++i)
			{
				const Vec2 relative = (points[i] - points[begin]);
				const double projection = relative.dot(direction);
				double distanceSq;
				if (projection <= 0.0)
				{
					distanceSq = relative.lengthSq();
				}
				else if (lengthSq <= projection)
				{
					distanceSq = points[i].distanceFromSq(endPoint);
				}
				else
				{
					distanceSq = (relative - direction * (projection / lengthSq)).lengthSq();
				}

				if (farthestDistanceSq < distanceSq)
				{
					farthestDistanceSq = distanceSq;
					split = i;
				}
			}

			if (split != begin)
			{
				pendingEnds.push_back(end);
				end = split;
				continue;
			}

			// Omit only the implicit closing point. Keep an explicit closing point
			// unless the entire result has collapsed to the starting point.
			if ((end != n) && not ((result.size() == 1) && (result.front() == endPoint)))
			{
				result.push_back(endPoint);
			}
			if (pendingEnds.isEmpty())
			{
				break;
			}
			begin = end;
			end = pendingEnds.back();
			pendingEnds.pop_back();
		}
	}
}

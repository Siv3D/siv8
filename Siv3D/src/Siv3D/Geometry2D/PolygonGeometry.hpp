//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# pragma once
# include <Siv3D/Polygon.hpp>

namespace s3d::detail
{
	// 入力順に辺を参照し、predicate が true を返した時点で終了する。
	// ゼロ長辺も渡す。呼び出し側で点として扱うか、除外するかを選ぶ。
	template <bool CloseRing, class Predicate>
	[[nodiscard]]
	bool AnyPolylineSegment(const std::span<const Vec2> points, Predicate&& predicate)
	{
		if (points.size() < 2)
		{
			return false;
		}

		for (size_t i = 0; (i + 1) < points.size(); ++i)
		{
			if (predicate(Line{ points[i], points[i + 1] }))
			{
				return true;
			}
		}

		if constexpr (CloseRing)
		{
			return predicate(Line{ points.back(), points.front() });
		}
		else
		{
			return false;
		}
	}

	template <class Predicate>
	[[nodiscard]]
	bool AnyPolygonEdge(const Polygon& polygon, Predicate&& predicate)
	{
		if (AnyPolylineSegment<true>(polygon.outer(), predicate))
		{
			return true;
		}

		for (const auto& hole : polygon.inners())
		{
			if (AnyPolylineSegment<true>(hole, predicate))
			{
				return true;
			}
		}

		return false;
	}
}

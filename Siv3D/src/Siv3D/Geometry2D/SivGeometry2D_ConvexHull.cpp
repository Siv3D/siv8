//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Geometry2D/ConvexHull.hpp>
# include <Siv3D/Geometry2D/BoundingRect.hpp>
# include <Siv3D/Polygon/TriangleFan.hpp>
# include <algorithm>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		bool IsPositiveTurn(const Point& a, const Point& b, const Point& c) noexcept
		{
			const int64 abx = (static_cast<int64>(b.x) - a.x);
			const int64 aby = (static_cast<int64>(b.y) - a.y);
			const int64 acx = (static_cast<int64>(c.x) - a.x);
			const int64 acy = (static_cast<int64>(c.y) - a.y);

			// 座標差の絶対値は UINT32_MAX 以下なので、積は uint64 に収まる。
			// 符号と絶対値を比較し、積の差でのオーバーフローと浮動小数点への丸めを避ける。
			const uint64 lhs = (static_cast<uint64>(Abs(abx)) * static_cast<uint64>(Abs(acy)));
			const uint64 rhs = (static_cast<uint64>(Abs(aby)) * static_cast<uint64>(Abs(acx)));
			const bool lhsNegative = (((abx < 0) != (acy < 0)) && (lhs != 0));
			const bool rhsNegative = (((aby < 0) != (acx < 0)) && (rhs != 0));

			if (lhsNegative != rhsNegative)
			{
				return rhsNegative;
			}

			return (lhsNegative ? (lhs < rhs) : (rhs < lhs));
		}

		template <class PointType>
		[[nodiscard]]
		bool IsPositiveTurn(const PointType& a, const PointType& b, const PointType& c) noexcept
		{
			const Vec2 ab = (Vec2{ b } - Vec2{ a });
			const Vec2 ac = (Vec2{ c } - Vec2{ a });
			// 積を同じ精度で丸めて比較する。一方の積だけが FMA と融合すると、
			// 共線な入力でも丸め誤差の符号を向きとして扱ってしまう。
			return ((ab.y * ac.x) < (ab.x * ac.y));
		}

		template <class PointType>
		[[nodiscard]]
		Polygon ConvexHull_impl(const std::span<const PointType> points)
		{
			if (points.size() < 3)
			{
				return{};
			}

			Array<PointType> sorted(points.begin(), points.end());
			std::sort(sorted.begin(), sorted.end(), [](const PointType& a, const PointType& b)
				{
					return ((a.x < b.x) || ((a.x == b.x) && (a.y < b.y)));
				});
			sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());

			if (sorted.size() < 3)
			{
				return{};
			}

			Array<PointType> hull{ Arg::reserve = (sorted.size() + 1) };

			for (const auto& point : sorted)
			{
				while ((2 <= hull.size()) && not IsPositiveTurn(hull[hull.size() - 2], hull.back(), point))
				{
					hull.pop_back();
				}

				hull.push_back(point);
			}

			const size_t upperBegin = (hull.size() + 1);

			for (size_t i = (sorted.size() - 1); i > 0; --i)
			{
				const auto& point = sorted[i - 1];

				while ((upperBegin <= hull.size()) && not IsPositiveTurn(hull[hull.size() - 2], hull.back(), point))
				{
					hull.pop_back();
				}

				hull.push_back(point);
			}

			hull.pop_back(); // 先頭頂点の重複を除く。

			if (hull.size() < 3)
			{
				return{};
			}

			Array<Vec2> outer;

			if constexpr (std::is_same_v<PointType, Vec2>)
			{
				outer = std::move(hull);
			}
			else
			{
				outer.assign_range(hull);
			}

			return Polygon{ outer, detail::MakeTriangleFan(outer.size()), Geometry2D::BoundingRect(outer), SkipValidation::Yes };
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	ConvexHull
		//
		////////////////////////////////////////////////////////////////
		
		Polygon ConvexHull(const std::span<const Point> points)
		{
			return ConvexHull_impl(points);
		}

		Polygon ConvexHull(const std::span<const Float2> points)
		{
			return ConvexHull_impl(points);
		}

		Polygon ConvexHull(const std::span<const Vec2> points)
		{
			return ConvexHull_impl(points);
		}
	}
}

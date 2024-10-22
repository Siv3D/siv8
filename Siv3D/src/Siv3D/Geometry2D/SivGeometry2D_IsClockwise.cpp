//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Geometry2D/IsClockwise.hpp>

namespace s3d
{
	namespace
	{
		template <class PointType>
		[[nodiscard]]
		static bool IsClockwise_impl(const std::span<const PointType> points)
		{
			const size_t size = points.size();

			// 頂点数が 2 以下の場合は判定できないため false を返す
			if (size <= 2)
			{
				return false;
			}

			const auto* const pSrcBegin = points.data();
			const auto* const pSrcEnd = (pSrcBegin + size);
			double sum = 0.0;

			// 最初の頂点 -> ... -> 最後の頂点
			for (const auto* pSrc = pSrcBegin; pSrc != (pSrcEnd - 1); ++pSrc)
			{
				const auto* p0 = pSrc;
				const auto* p1 = (pSrc + 1);
				sum += ((p1->x - p0->x) * (p1->y + p0->y));
			}

			// 最後の頂点 -> 最初の頂点
			{
				const auto* p0 = (pSrcEnd - 1);
				const auto* p1 = pSrcBegin;
				sum += ((p1->x - p0->x) * (p1->y + p0->y));
			}

			return (sum < 0.0);
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	IsClockwise
		//
		////////////////////////////////////////////////////////////////
		
		bool IsClockwise(const std::span<const Point> points) noexcept
		{
			return IsClockwise_impl(points);
		}

		bool IsClockwise(const std::span<const Float2> points) noexcept
		{
			return IsClockwise_impl(points);
		}

		bool IsClockwise(const std::span<const Vec2> points) noexcept
		{
			return IsClockwise_impl(points);
		}
	}
}

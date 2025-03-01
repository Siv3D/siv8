//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "../Common.hpp"
# include "../PointVector.hpp"

namespace s3d
{
	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	IsClockwise
		//
		////////////////////////////////////////////////////////////////

		template <class PointType>
		constexpr bool IsClockwise(const PointType& p0, const PointType& p1, const PointType& p2) noexcept
		{
			if constexpr (std::is_same_v<typename PointType::value_type, int32>)
			{
				return (0 < (static_cast<int64>(p1.x - p0.x) * static_cast<int64>(p2.y - p0.y) - static_cast<int64>(p2.x - p0.x) * static_cast<int64>(p1.y - p0.y)));
			}
			else
			{
				return (0 < ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y)));
			}
		}

		template <class PointType>
		constexpr bool IsClockwise(const PointType& p0, const PointType& p1, const PointType& p2, const PointType& p3) noexcept
		{
			if constexpr (std::is_same_v<typename PointType::value_type, int32>)
			{
				int64 sum = 0;
				sum += (static_cast<int64>(p1.x - p0.x) * static_cast<int64>(p1.y + p0.y));
				sum += (static_cast<int64>(p2.x - p1.x) * static_cast<int64>(p2.y + p1.y));
				sum += (static_cast<int64>(p3.x - p2.x) * static_cast<int64>(p3.y + p2.y));
				sum += (static_cast<int64>(p0.x - p3.x) * static_cast<int64>(p0.y + p3.y));
				return (sum < 0);
			}
			else
			{
				typename PointType::value_type sum = 0;
				sum += ((p1.x - p0.x) * (p1.y + p0.y));
				sum += ((p2.x - p1.x) * (p2.y + p1.y));
				sum += ((p3.x - p2.x) * (p3.y + p2.y));
				sum += ((p0.x - p3.x) * (p0.y + p3.y));
				return (sum < 0);
			}
		}
	}
}

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
		constexpr bool IsClockwise(const PointType& p0, const PointType& p1, const PointType& p2) noexcept;

		template <class PointType>
		constexpr bool IsClockwise(const PointType& p0, const PointType& p1, const PointType& p2, const PointType& p3) noexcept;
	}
}

# include "IsClockwise.ipp"

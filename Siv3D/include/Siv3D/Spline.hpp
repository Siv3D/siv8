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

# pragma once
# include "Common.hpp"

namespace s3d
{
	namespace Spline
	{
		template <class PointType, Concept::FloatingPoint Float = double>
		[[nodiscard]]
		constexpr PointType CalculateTangent(const PointType& p0, const PointType& p1, const PointType& p2, Float tension = Float{ 0 }, Float bias = Float{ 0 }) noexcept;

		template <class PointType>
		[[nodiscard]]
		constexpr PointType Hermite(const PointType& p0, const PointType& p1, const PointType& m0, const PointType& m1, Concept::FloatingPoint auto t) noexcept;

		template <class PointType>
		[[nodiscard]]
		constexpr PointType CatmullRom(const PointType& p0, const PointType& p1, const PointType& p2, const PointType& p3, Concept::FloatingPoint auto t) noexcept;
	}
}

# include "detail/Spline.ipp"

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

namespace s3d
{
	namespace Spline
	{
		template <class PointType, Concept::FloatingPoint Float>
		constexpr PointType CalculateTangent(const PointType& p0, const PointType& p1, const PointType& p2, const Float tension, const Float bias) noexcept
		{
			const auto k = ((1 - tension) / 2);
			const auto w0 = (1 + bias);
			const auto w1 = (1 - bias);
			return ((k * w0) * (p1 - p0) + (k * w1) * (p2 - p1));
		}

		template <class PointType>
		constexpr PointType Hermite(const PointType& p0, const PointType& p1, const PointType& m0, const PointType& m1, const Concept::FloatingPoint auto t) noexcept
		{
			const auto t2 = (t * t);
			const auto t3 = (t2 * t);
			const auto h00 = (2 * t3 - 3 * t2 + 1);
			const auto h10 = (t3 - 2 * t2 + t);
			const auto h01 = (-2 * t3 + 3 * t2);
			const auto h11 = (t3 - t2);
			return ((h00 * p0) + (h10 * m0) + (h11 * m1) + (h01 * p1));
		}

		template <class PointType>
		constexpr PointType CatmullRom(const PointType& p0, const PointType& p1, const PointType& p2, const PointType& p3, const Concept::FloatingPoint auto t) noexcept
		{
			const auto m0 = CalculateTangent(p0, p1, p2);
			const auto m1 = CalculateTangent(p1, p2, p3);
			return Hermite(p1, p2, m0, m1, t);
		}
	}
}

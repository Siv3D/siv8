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

		/// @brief 3 点が時計回りの順序で並んでいるかを返します。
		/// @tparam PointType 座標の型
		/// @param p0 点 0
		/// @param p1 点 1
		/// @param p2 点 2
		/// @return 3 点が時計回りの順序で並んでいる場合 true, それ以外の場合は false
		template <class PointType>
		[[nodiscard]]
		constexpr bool IsClockwise(const PointType& p0, const PointType& p1, const PointType& p2) noexcept;

		/// @brief 4 点が時計回りの順序で並んでいるかを返します。
		/// @tparam PointType 座標の型
		/// @param p0 点 0
		/// @param p1 点 1
		/// @param p2 点 2
		/// @param p3 点 3
		/// @return 4 点が時計回りの順序で並んでいる場合 true, それ以外の場合は false
		template <class PointType>
		[[nodiscard]]
		constexpr bool IsClockwise(const PointType& p0, const PointType& p1, const PointType& p2, const PointType& p3) noexcept;

		/// @brief 一連の点が時計回りの順序で並んでいるかを返します。
		/// @param points 一連の点
		/// @return 一連の点が時計回りの順序で並んでいる場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsClockwise(std::span<const Point> points) noexcept;
		
		/// @brief 一連の点が時計回りの順序で並んでいるかを返します。
		/// @param points 一連の点
		/// @return 一連の点が時計回りの順序で並んでいる場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsClockwise(std::span<const Float2> points) noexcept;
		
		/// @brief 一連の点が時計回りの順序で並んでいるかを返します。
		/// @param points 一連の点
		/// @return 一連の点が時計回りの順序で並んでいる場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsClockwise(std::span<const Vec2> points) noexcept;
	}
}

# include "IsClockwise.ipp"

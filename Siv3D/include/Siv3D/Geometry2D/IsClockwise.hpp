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
# include "../Common.hpp"
# include "../PointVector.hpp"

namespace s3d
{
	namespace Geometry2D
	{
		/// @note このヘッダの時計回りは画面座標（X 右向き、Y 下向き）での向きです。
		/// @note 点列版では符号付き面積の 2 倍 `Σ(x[i] * y[i+1] - x[i+1] * y[i])` が正の場合に true を返します。Polygon の外周と Loft の断面に対応します。頂点数が 2 以下、または面積の和が 0 の場合は false です。
		/// @note 自己交差や重複点の有無を検査する関数ではありません。Polygon の入力診断には Polygon::Validate() を使います。
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

# include "detail/IsClockwise.ipp"

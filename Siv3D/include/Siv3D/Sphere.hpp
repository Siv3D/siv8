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
# include "PointVector.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Sphere
	//
	////////////////////////////////////////////////////////////////

	/// @brief 球
	struct Sphere
	{
		/// @brief 球の中心座標を表現する型
		using position_type = Vec3;

		/// @brief 球の半径を表現する型
		using size_type = double;

		/// @brief 球の座標や半径の成分の型
		using value_type = double;

		/// @brief 球の中心座標
		position_type center;

		/// @brief 球の半径
		size_type r;

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Sphere() = default;

		/// @brief 原点を中心とする球を作成します。
		/// @param _r 球の半径
		[[nodiscard]]
		explicit constexpr Sphere(const size_type _r) noexcept
			: center{ 0.0, 0.0, 0.0 }
			, r{ _r } {}

		/// @brief 球を作成します。
		/// @param _center 球の中心座標
		/// @param _r 球の半径
		[[nodiscard]]
		constexpr Sphere(const position_type _center, const size_type _r) noexcept
			: center{ _center }
			, r{ _r } {}

		/// @brief 2 つの球が等しいかを返します。
		/// @param lhs 左辺の球
		/// @param rhs 右辺の球
		/// @return 中心座標と半径がそれぞれ等しい場合 true, それ以外の場合は false
		friend constexpr bool operator ==(const Sphere& lhs, const Sphere& rhs) = default;
	};
}

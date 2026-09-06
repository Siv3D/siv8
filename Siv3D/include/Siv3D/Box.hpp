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
	//	Box
	//
	////////////////////////////////////////////////////////////////

	/// @brief 軸に平行な直方体
	struct Box
	{
		/// @brief 直方体の中心座標を表現する型
		using position_type = Vec3;

		/// @brief 直方体の大きさを表現する型
		using size_type = Vec3;

		/// @brief 直方体の座標や大きさの成分の型
		using value_type = size_type::value_type;

		/// @brief 直方体の中心座標
		position_type center;

		/// @brief 直方体の各軸方向の大きさ
		size_type size;

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Box() = default;

		/// @brief 原点を中心とする直方体を作成します。
		/// @param _size 直方体の各軸方向の大きさ
		[[nodiscard]]
		explicit constexpr Box(const value_type _size) noexcept
			: center{ 0.0, 0.0, 0.0 }
			, size{ _size, _size, _size } {}

		/// @brief 原点を中心とする直方体を作成します。
		/// @param _size 直方体の各軸方向の大きさ
		[[nodiscard]]
		explicit constexpr Box(const size_type _size) noexcept
			: center{ 0.0, 0.0, 0.0 }
			, size{ _size } {}

		/// @brief 直方体を作成します。
		/// @param _center 直方体の中心座標
		/// @param _size 直方体の各軸方向の大きさ
		[[nodiscard]]
		constexpr Box(const position_type _center, const value_type _size) noexcept
			: center{ _center }
			, size{ _size, _size, _size } {}

		/// @brief 直方体を作成します。
		/// @param _center 直方体の中心座標
		/// @param _size 直方体の各軸方向の大きさ
		[[nodiscard]]
		constexpr Box(const position_type _center, const size_type _size) noexcept
			: center{ _center }
			, size{ _size } {}

		/// @brief 2 つの直方体が等しいかを返します。
		/// @param lhs 左辺の直方体
		/// @param rhs 右辺の直方体
		/// @return 中心座標と大きさがそれぞれ等しい場合 true, それ以外の場合は false
		friend constexpr bool operator ==(const Box& lhs, const Box& rhs) = default;
	};
}

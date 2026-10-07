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
# include <cassert>
# include "PointVector.hpp"

namespace s3d
{
	/// @brief 始点と単位方向ベクトルで表す半直線
	struct Ray2D
	{
		/// @brief 始点（有限の座標）
		Vec2 origin{ 0.0, 0.0 };
		
		/// @brief 単位方向ベクトル。直接代入する場合も長さ 1 に保ちます。
		Vec2 direction{ 1.0, 0.0 };

		[[nodiscard]]
		constexpr Ray2D() noexcept = default;

		/// @brief 指定した向きを正規化したレイを作成します。
		/// @param _origin 始点
		/// @param _direction 向き
		/// @pre _origin は有限、_direction の長さの二乗は有限かつ正である必要があります。
		[[nodiscard]]
		Ray2D(const Vec2& _origin, const Vec2& _direction) noexcept;

		/// @brief 始点から指定距離だけ進んだ位置を返します。
		/// @param distance 距離。負の場合は始点の後方です。
		[[nodiscard]]
		constexpr Vec2 pointAt(const double distance) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const Ray2D& value);
	};
}

////////////////////////////////////////////////////////////////
//
//	fmt
//
////////////////////////////////////////////////////////////////

template <>
struct fmt::formatter<s3d::Ray2D, s3d::char32>
{
	std::u32string tag;

	s3d::ParseContext::iterator parse(s3d::ParseContext& ctx);

	s3d::BufferContext::iterator format(const s3d::Ray2D& value, s3d::BufferContext& ctx) const;
};

# include "detail/Ray2D.ipp"

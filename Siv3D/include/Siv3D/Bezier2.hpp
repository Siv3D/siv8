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
	/// @brief 二次ベジェ曲線
	struct Bezier2
	{
		using position_type = Vec2;

		position_type p0;

		position_type p1;

		position_type p2;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Bezier2() = default;

		[[nodiscard]]
		constexpr Bezier2(const position_type& _p0, const position_type& _p1, const position_type& _p2) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pointAtIndex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		position_type& pointAtIndex(size_t index);

		[[nodiscard]]
		const position_type& pointAtIndex(size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	getPos
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr position_type getPos(const double t) const noexcept
		{
			return ((1 - t) * (1 - t) * p0 + 2 * (1 - t) * t * p1 + t * t * p2);
		}

		////////////////////////////////////////////////////////////////
		//
		//	getTangent
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vec2 getTangent(double t) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getLineString
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		LineString getLineString(const int32 quality = 24) const;

		[[nodiscard]]
		LineString getLineString(double start, double end, int32 quality = 24) const;

		////////////////////////////////////////////////////////////////
		//
		//	boundingRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		RectF boundingRect() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		const Bezier2& draw(const ColorF& color = Palette::White, int32 quality = 24) const;

		const Bezier2& draw(double thickness, const ColorF& color = Palette::White, int32 quality = 24) const;

		const Bezier2& draw(LineCap lineCap, double thickness, const ColorF& color = Palette::White, int32 quality = 24) const;

	};
}



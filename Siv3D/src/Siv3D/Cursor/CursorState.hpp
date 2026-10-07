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
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>
# include "CursorTransform.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	CursorState
	//
	////////////////////////////////////////////////////////////////

	struct CursorState
	{
		template <class VectorType>
		struct Internal
		{
			VectorType previous{ 0,0 };
			VectorType current{ 0,0 };
			VectorType delta{ 0,0 };

			constexpr void update(const VectorType& newPos) noexcept
			{
				previous	= current;
				current		= newPos;
				delta		= (current - previous);
			}
		};

		/// @brief スクリーン座標
		Internal<Point> screen;

		/// @brief 補正前のクライアント座標
		Internal<Point> raw;
		
		/// @brief 補正済みのクライアント座標
		Internal<Vec2>	vec2;
		
		/// @brief vec2 の整数座標
		Internal<Point> point;

		constexpr void advanceRaw(const Point screenPos, const Point rawPos) noexcept
		{
			screen.update(screenPos);
			raw.update(rawPos);
		}

		constexpr void refreshTransformed(const CursorTransform& transform) noexcept
		{
			vec2.previous = transform.allInv.transformPoint(Vec2{ raw.previous });
			vec2.current = transform.allInv.transformPoint(Vec2{ raw.current });
			vec2.delta = (vec2.current - vec2.previous);

			point.previous = vec2.previous.asPoint();
			point.current = vec2.current.asPoint();
			point.delta = (point.current - point.previous);
		}
	};
}

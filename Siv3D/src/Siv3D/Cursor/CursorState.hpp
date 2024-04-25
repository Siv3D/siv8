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
# include <Siv3D/Common.hpp>
# include <Siv3D/PointVector.hpp>

namespace s3d
{
	struct CursorState
	{
		template <class VectrorType>
		struct Internal
		{
			VectrorType previous{ 0,0 };
			VectrorType current{ 0,0 };
			VectrorType delta{ 0,0 };

			constexpr void update(const VectrorType& newPos) noexcept
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

		constexpr void update(const Point& _screen, const Point& _raw, const Vec2& _vec2) noexcept
		{
			screen.update(_screen);
			raw.update(_raw);
			vec2.update(_vec2);
			point.update(_vec2.asPoint());
		}
	};
}

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
# include "../Array.hpp"
# include "../Polygon.hpp"

namespace s3d
{
	namespace Geometry2D
	{
		//////////////////////////////////////////////////
		//
		//	And
		//
		//////////////////////////////////////////////////

		/// @brief 2 つの図形の重なる領域を返します。
		/// @param a 図形
		/// @param b 他方の図形
		/// @return 2 つの図形の重なる領域
		[[nodiscard]]
		Array<Polygon> And(const RectF& a, const Polygon& b);

		/// @brief 2 つの図形の重なる領域を返します。
		/// @param a 図形
		/// @param b 他方の図形
		/// @return 2 つの図形の重なる領域
		[[nodiscard]]
		Array<Polygon> And(const Polygon& a, const RectF& b);

		/// @brief 2 つの図形の重なる領域を返します。
		/// @param a 図形
		/// @param b 他方の図形
		/// @return 2 つの図形の重なる領域
		[[nodiscard]]
		Array<Polygon> And(const Polygon& a, const Polygon& b);
	}
}

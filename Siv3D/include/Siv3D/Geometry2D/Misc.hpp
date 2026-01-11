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
# include "../MultiPolygon.hpp"

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





		//////////////////////////////////////////////////
		//
		//	ComposePolygons
		//
		//////////////////////////////////////////////////

		/// @brief 入力されたリングをもとに、多角形 `Polygon` を組み立てます。
		/// @param rings リングの配列。穴になるリングは反時計回り、外側になるリングは時計回りにします。
		/// @return 組み立て得られた多角形 `Polygon` の配列
		[[nodiscard]]
		MultiPolygon ComposePolygons(const Array<LineString>& rings);
	}
}

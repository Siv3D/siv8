
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
# include "../Optional.hpp"
# include "../2DShapes.hpp"

namespace s3d
{
	namespace Geometry2D
	{
		/// @defgroup geometry2d_boundary_distance 境界最近点・符号付き距離
		/// @brief ClosestPointOnBoundary() / SignedDistance() の共通契約。
		/// @pre 図形の入力条件と空・縮退の分類は @ref geometry2d_queries に従います。point の座標は有限値とします。
		///
		/// | 関数 | 戻り値 | shape が空の場合 |
		/// |---|---|---|
		/// | ClosestPointOnBoundary(shape, point) | point に最も近い境界上の点。 | none。 |
		/// | SignedDistance(shape, point) | 境界までの距離。内部は負、境界上は 0、外部は正。 | 正の無限大。 |
		///
		/// Polygon の外周と穴の輪郭を境界とし、穴の内部は外部として扱います。MultiPolygon は空要素を無視し、各要素の境界を対象にします。
		/// 点・線分に縮退した図形では、その全体を境界とし、符号付き距離は非負です。
		/// 最近点が一意でない場合、どの点を返すかは未規定です。
		///
		/// @par 数値計算
		/// SuperEllipse の最近点と距離は数値近似です。
		/// SignedDistance() は境界のごく近傍を 0 とする場合があり、その許容誤差は座標のスケールに依存します。
		/// @see @ref geometry2d_distance
		/// @{

		template <class Shape, class PointType>
		Optional<Vec2> ClosestPointOnBoundary(const Shape&, const PointType&) = delete;

		template <class Shape, class PointType>
		double SignedDistance(const Shape&, const PointType&) = delete;

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Rect, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Rect& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Rect& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Rect& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Rect& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(RectF, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const RectF& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const RectF& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const RectF& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const RectF& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Circle, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Circle& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Circle& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Circle& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Circle& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Ellipse, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Ellipse& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Ellipse& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Ellipse& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Ellipse& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(SuperEllipse, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const SuperEllipse& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const SuperEllipse& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const SuperEllipse& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const SuperEllipse& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Triangle, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Triangle& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Triangle& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Triangle& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Triangle& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Quad, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Quad& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Quad& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Quad& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Quad& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(RoundRect, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const RoundRect& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const RoundRect& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const RoundRect& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const RoundRect& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Polygon, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Polygon& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const Polygon& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const Polygon& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const Polygon& shape, const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(MultiPolygon, point)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const MultiPolygon& shape, const Point& point);

		[[nodiscard]]
		double SignedDistance(const MultiPolygon& shape, const Point& point);

		[[nodiscard]]
		Optional<Vec2> ClosestPointOnBoundary(const MultiPolygon& shape, const Vec2& point);

		[[nodiscard]]
		double SignedDistance(const MultiPolygon& shape, const Vec2& point);

		/// @}
	}
}

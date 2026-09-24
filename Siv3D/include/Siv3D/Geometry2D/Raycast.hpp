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
# include "../Number.hpp"
# include "../2DShapes.hpp"
# include "../Ray2D.hpp"
# include "../RaycastHit2D.hpp"

namespace s3d
{
	class Polygon;
	class MultiPolygon;

	namespace Geometry2D
	{
		template <class Shape>
		Optional<RaycastHit2D> Raycast(const Ray2D&, const Shape&, double) = delete;

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Rect& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const RectF& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Circle& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Ellipse& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const SuperEllipse& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Triangle& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Quad& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const RoundRect& shape, double maxDistance = Math::Inf);

		/// @brief レイと多角形の境界との最初のヒットを取得します。
		/// @param ray レイ
		/// @param shape 多角形
		/// @param maxDistance レイ始点からの最大距離
		/// @return ヒット情報。多角形が面積を持たない場合、または指定範囲内にヒットがない場合は none
		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Polygon& shape, double maxDistance = Math::Inf);

		/// @brief レイと複数の多角形の境界との最初のヒットを取得します。
		/// @param ray レイ
		/// @param shape 複数の多角形
		/// @param maxDistance レイ始点からの最大距離
		/// @return ヒット情報。指定範囲内にヒットがない場合は none
		/// @remark 空または点・線分に縮退した要素は、ヒットと startsInside の判定から除外します。
		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const MultiPolygon& shape, double maxDistance = Math::Inf);
	}
}

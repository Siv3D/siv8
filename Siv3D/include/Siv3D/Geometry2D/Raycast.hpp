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

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Polygon& shape, double maxDistance = Math::Inf);

		[[nodiscard]]
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const MultiPolygon& shape, double maxDistance = Math::Inf);
	}
}

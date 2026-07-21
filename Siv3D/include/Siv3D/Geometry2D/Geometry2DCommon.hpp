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
# include "../2DShapes.hpp"

namespace s3d
{
	namespace detail
	{
		// Shared semantic classification for valid size-parameterized 2D shapes.
		// Cheap local validity checks are Debug assertions only. NaN / infinity
		// checks remain outside predicate hot paths.
		enum class Geometry2DSizedShapeKind : uint8
		{
			Empty,
			VerticalSegment,
			HorizontalSegment,
			Area,
		};

		[[nodiscard]]
		constexpr bool IsGeometry2DSegment(const Geometry2DSizedShapeKind kind) noexcept;

		[[nodiscard]]
		constexpr Geometry2DSizedShapeKind ClassifyGeometry2DSizedShape(const RectF& rect) noexcept;

		[[nodiscard]]
		constexpr Geometry2DSizedShapeKind ClassifyGeometry2DSizedShape(const Circle& circle) noexcept;

		[[nodiscard]]
		constexpr Geometry2DSizedShapeKind ClassifyGeometry2DSizedShape(const Ellipse& ellipse) noexcept;

		[[nodiscard]]
		constexpr Geometry2DSizedShapeKind ClassifyGeometry2DSizedShape(const SuperEllipse& superEllipse) noexcept;

		[[nodiscard]]
		constexpr Geometry2DSizedShapeKind ClassifyGeometry2DSizedShape(const RoundRect& roundRect) noexcept;

		// `kind` must be VerticalSegment or HorizontalSegment.
		[[nodiscard]]
		constexpr Line GetGeometry2DDegenerateSegment(const RectF& rect, Geometry2DSizedShapeKind kind) noexcept;

		[[nodiscard]]
		constexpr Line GetGeometry2DDegenerateSegment(const Ellipse& ellipse, Geometry2DSizedShapeKind kind) noexcept;

		[[nodiscard]]
		constexpr Line GetGeometry2DDegenerateSegment(const SuperEllipse& superEllipse, Geometry2DSizedShapeKind kind) noexcept;

		[[nodiscard]]
		constexpr Line GetGeometry2DDegenerateSegment(const RoundRect& roundRect, Geometry2DSizedShapeKind kind) noexcept;

		[[nodiscard]]
		constexpr double GetGeometry2DEffectiveRadius(const RoundRect& roundRect) noexcept;

		[[nodiscard]]
		constexpr RectF GetGeometry2DRoundRectCore(const RoundRect& roundRect, double effectiveRadius) noexcept;
	}
}

# include "detail/Geometry2DCommon.ipp"

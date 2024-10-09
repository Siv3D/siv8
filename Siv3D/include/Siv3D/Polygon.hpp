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
# include <memory>
# include "Array.hpp"
# include "Common.hpp"
# include "PointVector.hpp"
# include "2DShapes.hpp"
# include "TriangleIndex.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	class Polygon
	{
	private:

		class PolygonDetail;

	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Polygon();

		[[nodiscard]]
		Polygon(const Polygon& other);

		[[nodiscard]]
		Polygon(Polygon&& other) noexcept;

		[[nodiscard]]
		Polygon(std::span<const Vec2> outer, std::span<const std::span<const Vec2>> holes = {}, SkipValidation skipValidation = SkipValidation::No);

		[[nodiscard]]
		Polygon(std::span<const Vec2> outer, std::span<const TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		[[nodiscard]]
		Polygon(std::span<const Vec2> outer, std::span<const std::span<const Vec2>> holes, std::span<const Float2> vertices, std::span<const TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation = SkipValidation::No);

		[[nodiscard]]
		explicit Polygon(std::initializer_list<Vec2> outer, SkipValidation skipValidation = SkipValidation::No);

		[[nodiscard]]
		Polygon(const Shape2D& shape);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~Polygon();

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		Polygon& operator =(const Polygon& other);

		Polygon& operator =(Polygon&& other) noexcept;

	private:

		std::unique_ptr<PolygonDetail> pImpl;
	};
}

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

# include <Siv3D/Polygon.hpp>
# include "PolygonDetail.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Polygon::Polygon()
		: pImpl{ std::make_unique<PolygonDetail>() } {}

	Polygon::Polygon(const Polygon& polygon)
		: pImpl{ std::make_unique<PolygonDetail>(*polygon.pImpl) } {}

	Polygon::Polygon(Polygon&& polygon) noexcept
		: pImpl{ std::exchange(polygon.pImpl, std::make_unique<PolygonDetail>()) } {}

	Polygon::Polygon(std::span<const Vec2> outer, std::span<const std::span<const Vec2>> holes, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, holes, skipValidation) } {}

	Polygon::Polygon(std::span<const Vec2> outer, std::span<const TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, indices, boundingRect, skipValidation) } {}

	Polygon::Polygon(std::span<const Vec2> outer, std::span<const std::span<const Vec2>> holes, std::span<const Float2> vertices, std::span<const TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, holes, vertices, indices, boundingRect, skipValidation) } {}

	Polygon::Polygon(std::initializer_list<Vec2> outer, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::span<const std::span<const Vec2>>{}, skipValidation) } {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	Polygon::~Polygon() = default;

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	Polygon& Polygon::operator =(const Polygon& other)
	{
		if (&other != this)
		{
			*pImpl = *other.pImpl;
		}
	
		return *this;
	}

	Polygon& Polygon::operator =(Polygon&& other) noexcept
	{
		if (&other != this)
		{
			pImpl = std::exchange(other.pImpl, std::make_unique<PolygonDetail>());
		}

		return *this;
	}
}

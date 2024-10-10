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

	Polygon::Polygon(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(holes), skipValidation) } {}

	Polygon::Polygon(const std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(indices), boundingRect, skipValidation) } {}

	Polygon::Polygon(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, std::move(holes), std::move(vertices), std::move(indices), boundingRect, skipValidation) } {}

	Polygon::Polygon(std::initializer_list<Vec2> outer, const SkipValidation skipValidation)
		: pImpl{ std::make_unique<PolygonDetail>(outer, Array<Array<Vec2>>{}, skipValidation) } {}

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

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::isEmpty() const noexcept
	{
		return pImpl->isEmpty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	Polygon::operator bool() const noexcept
	{
		return (not pImpl->isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasHoles
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::hasHoles() const noexcept
	{
		return (not pImpl->inners().empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_holes
	//
	////////////////////////////////////////////////////////////////

	size_t Polygon::num_holes() const noexcept
	{
		return pImpl->inners().size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	outer
	//
	////////////////////////////////////////////////////////////////

	const Array<Vec2>& Polygon::outer() const noexcept
	{
		return pImpl->outer();
	}

	////////////////////////////////////////////////////////////////
	//
	//	inners
	//
	////////////////////////////////////////////////////////////////

	const Array<Array<Vec2>>& Polygon::inners() const noexcept
	{
		return pImpl->inners();
	}

	////////////////////////////////////////////////////////////////
	//
	//	vertices
	//
	////////////////////////////////////////////////////////////////

	const Array<Float2>& Polygon::vertices() const noexcept
	{
		return pImpl->vertices();
	}

	////////////////////////////////////////////////////////////////
	//
	//	indices
	//
	////////////////////////////////////////////////////////////////

	const Array<TriangleIndex>& Polygon::indices() const noexcept
	{
		return pImpl->indices();
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	const RectF& Polygon::boundingRect() const noexcept
	{
		return pImpl->boundingRect();
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_triangles
	//
	////////////////////////////////////////////////////////////////

	size_t Polygon::num_triangles() const noexcept
	{
		return pImpl->indices().size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	triangle
	//
	////////////////////////////////////////////////////////////////

	Triangle Polygon::triangleAtIndex(const size_t index) const
	{
		const auto& indices = pImpl->indices();

		if (indices.size() <= index)
		{
			ThrowTriangleAtIndexOutOfRange();
		}

		const Float2* pVertices = pImpl->vertices().data();
		const auto& triangleIndex = indices[index];

		return{ pVertices[triangleIndex.i0], pVertices[triangleIndex.i1], pVertices[triangleIndex.i2] };
	}





	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Polygon& Polygon::draw(const ColorF& color) const
	{
		pImpl->draw(color);
		return *this;
	}

	void Polygon::draw(const double x, const double y, const ColorF& color) const
	{
		pImpl->draw(Vec2{ x, y }, color);
	}

	void Polygon::draw(const Vec2& pos, const ColorF& color) const
	{
		pImpl->draw(pos, color);
	}

	const Polygon& Polygon::draw(const PatternParameters& pattern) const
	{
		pImpl->draw(pattern);
		return *this;
	}

	void Polygon::draw(double x, double y, const PatternParameters& pattern) const
	{
		pImpl->draw(Vec2{ x, y }, pattern);
	}

	void Polygon::draw(const Vec2& pos, const PatternParameters& pattern) const
	{
		pImpl->draw(pos, pattern);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Validate
	//
	////////////////////////////////////////////////////////////////

	PolygonFailureType Polygon::Validate(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
	{
		return PolygonDetail::Validate(outer, holes);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	void Polygon::ThrowTriangleAtIndexOutOfRange()
	{
		throw std::out_of_range{ "Polygon::triangleAtIndex() index out of range" };
	}
}

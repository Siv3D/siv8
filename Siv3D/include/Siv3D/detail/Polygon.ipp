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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	inline void Polygon::swap(Polygon& other) noexcept
	{
		pImpl.swap(other.pImpl);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceTo
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	double Polygon::distanceTo(const Shape2DType& other) const
	{
		return Geometry2D::Distance(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool Polygon::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersects(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	overlaps
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool Polygon::overlaps(const Shape2DType& other) const
	{
		return Geometry2D::Overlaps(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool Polygon::contains(const Shape2DType& other) const
	{
		return Geometry2D::Contains(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersectsAt
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	Optional<Array<Vec2>> Polygon::intersectsAt(const Shape2DType& other) const
	{
		return Geometry2D::IntersectsAt(*this, other);
	}
}

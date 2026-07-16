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
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse::SuperEllipse(const value_type _x, const value_type _y, const value_type _a, const value_type _b, const value_type _n) noexcept
		: center{ _x, _y }
		, axes{ _a, _b }
		, n{ _n } {}

	constexpr SuperEllipse::SuperEllipse(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _a, const Concept::Arithmetic auto _b, const Concept::Arithmetic auto _n) noexcept
		: center{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, axes{ static_cast<value_type>(_a), static_cast<value_type>(_b) }
		, n{ static_cast<value_type>(_n) } {}

	constexpr SuperEllipse::SuperEllipse(const position_type& _center, const value_type _a, const value_type _b, const value_type _n) noexcept
		: center{ _center }
		, axes{ _a, _b }
		, n{ _n } {}

	constexpr SuperEllipse::SuperEllipse(const position_type& _center, const Concept::Arithmetic auto _a, const Concept::Arithmetic auto _b, const Concept::Arithmetic auto _n) noexcept
		: center{ _center }
		, axes{ static_cast<value_type>(_a), static_cast<value_type>(_b) }
		, n{ static_cast<value_type>(_n) } {}

	constexpr SuperEllipse::SuperEllipse(const value_type _x, const value_type _y, const size_type& _axes, const value_type _n) noexcept
		: center{ _x, _y }
		, axes{ _axes }
		, n{ _n } {}

	constexpr SuperEllipse::SuperEllipse(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const size_type& _axes, const Concept::Arithmetic auto _n) noexcept
		: center{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, axes{ _axes }
		, n{ static_cast<value_type>(_n) } {}

	constexpr SuperEllipse::SuperEllipse(const position_type& _center, const size_type& _axes, const Concept::Arithmetic auto _n) noexcept
		: center{ _center }
		, axes{ _axes }
		, n{ static_cast<value_type>(_n) } {}

	constexpr SuperEllipse::SuperEllipse(const Ellipse& ellipse, const Concept::Arithmetic auto _n) noexcept
		: center{ ellipse.center }
		, axes{ ellipse.axes }
		, n{ static_cast<value_type>(_n) } {}

	constexpr SuperEllipse::SuperEllipse(const RectF& rect, const Concept::Arithmetic auto _n) noexcept
		: center{ rect.center()}
		, axes{ (rect.size.x * 0.5), (rect.size.y * 0.5) }
		, n{ static_cast<value_type>(_n) } {}

	////////////////////////////////////////////////////////////////
	//
	//	withX
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withX(const value_type _x) const noexcept
	{
		return{ _x, center.y, axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withY
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withY(const value_type _y) const noexcept
	{
		return{ center.x, _y, axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withCenter(const value_type _x, const value_type _y) const noexcept
	{
		return{ _x, _y, axes, n };
	}

	constexpr SuperEllipse SuperEllipse::withCenter(const position_type _center) const noexcept
	{
		return{ _center, axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withA
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withA(const value_type _a) const noexcept
	{
		return{ center, _a, axes.y, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withB
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withB(const value_type _b) const noexcept
	{
		return{ center, axes.x, _b, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withAxes
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withAxes(const value_type _a, const value_type _b) const noexcept
	{
		return{ center, _a, _b, n };
	}

	constexpr SuperEllipse SuperEllipse::withAxes(const size_type _axes) const noexcept
	{
		return{ center, _axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withN
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withN(const value_type _n) const noexcept
	{
		return{ center, axes, _n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse& SuperEllipse::setPos(const value_type _x, const value_type _y) noexcept
	{
		center.set(_x, _y);
		return *this;
	}

	constexpr SuperEllipse& SuperEllipse::setPos(const position_type _center) noexcept
	{
		center = _center;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse& SuperEllipse::setCenter(const value_type _x, const value_type _y) noexcept
	{
		center.set(_x, _y);
		return *this;
	}

	constexpr SuperEllipse& SuperEllipse::setCenter(const position_type _center) noexcept
	{
		center = _center;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setA
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse& SuperEllipse::setA(const value_type _a) noexcept
	{
		axes.x = _a;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setB
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse& SuperEllipse::setB(const value_type _b) noexcept
	{
		axes.y = _b;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setAxes
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse& SuperEllipse::setAxes(const value_type _a, const value_type _b) noexcept
	{
		axes.set(_a, _b);
		return *this;
	}

	constexpr SuperEllipse& SuperEllipse::setAxes(const size_type _axes) noexcept
	{
		axes = _axes;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setN
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse& SuperEllipse::setN(const value_type _n) noexcept
	{
		n = _n;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse& SuperEllipse::set(const value_type _x, const value_type _y, const value_type _a, const value_type _b, const value_type _n) noexcept
	{
		center.set(_x, _y);
		axes.set(_a, _b);
		n = _n;
		return *this;
	}

	constexpr SuperEllipse& SuperEllipse::set(const position_type& _center, const value_type _a, const value_type _b, const value_type _n) noexcept
	{
		center = _center;
		axes.set(_a, _b);
		n = _n;
		return *this;
	}

	constexpr SuperEllipse& SuperEllipse::set(const value_type _x, const value_type _y, const size_type& _axes, const value_type _n) noexcept
	{
		center.set(_x, _y);
		axes = _axes;
		n = _n;
		return *this;
	}

	constexpr SuperEllipse& SuperEllipse::set(const position_type& _center, const size_type& _axes, const value_type _n) noexcept
	{
		center = _center;
		axes = _axes;
		n = _n;
		return *this;
	}

	constexpr SuperEllipse& SuperEllipse::set(const SuperEllipse& ellipse) noexcept
	{
		center = ellipse.center;
		axes = ellipse.axes;
		n = ellipse.n;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::movedBy(const value_type _x, const value_type _y) const noexcept
	{
		return{ center.movedBy(_x, _y), axes, n };
	}

	constexpr SuperEllipse SuperEllipse::movedBy(const position_type v) const noexcept
	{
		return{ center.movedBy(v), axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse& SuperEllipse::moveBy(const value_type _x, const value_type _y) noexcept
	{
		center.moveBy(_x, _y);
		return *this;
	}

	constexpr SuperEllipse& SuperEllipse::moveBy(const position_type v) noexcept
	{
		center.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffset
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withOffset(const value_type _x, const value_type _y) const noexcept
	{
		return{ center.withOffset(_x, _y), axes, n };
	}

	constexpr SuperEllipse SuperEllipse::withOffset(const position_type v) const noexcept
	{
		return{ center.withOffset(v), axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffsetX, withOffsetY
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::withOffsetX(const value_type _x) const noexcept
	{
		return{ center.withOffsetX(_x), axes, n };
	}

	constexpr SuperEllipse SuperEllipse::withOffsetY(const value_type _y) const noexcept
	{
		return{ center.withOffsetY(_y), axes, n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	stretched
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::stretched(const value_type size) const noexcept
	{
		return{ center, (axes.x + size), (axes.y + size), n };
	}

	constexpr SuperEllipse SuperEllipse::stretched(const double _x, const double _y) const noexcept
	{
		return{ center, (axes.x + _x), (axes.y + _y), n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaled
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::scaled(const double s) const noexcept
	{
		return{ center, (axes.x * s), (axes.y * s), n };
	}

	constexpr SuperEllipse SuperEllipse::scaled(const double sx, const double sy) const noexcept
	{
		return{ center, (axes.x * sx), (axes.y * sy), n };
	}

	////////////////////////////////////////////////////////////////
	//
	//	top, right, bottom, left
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse::position_type SuperEllipse::top() const noexcept
	{
		return{ center.x, (center.y - axes.y) };
	}

	constexpr SuperEllipse::position_type SuperEllipse::right() const noexcept
	{
		return{ (center.x + axes.x), center.y };
	}

	constexpr SuperEllipse::position_type SuperEllipse::bottom() const noexcept
	{
		return{ center.x, (center.y + axes.y) };
	}

	constexpr SuperEllipse::position_type SuperEllipse::left() const noexcept
	{
		return{ (center.x - axes.x), center.y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	horizontalDiameter, verticalDiameter
	//
	////////////////////////////////////////////////////////////////

	constexpr Line SuperEllipse::horizontalDiameter() const noexcept
	{
		return{ left(), right() };
	}

	constexpr Line SuperEllipse::verticalDiameter() const noexcept
	{
		return{ top(), bottom() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingCircle
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle SuperEllipse::boundingCircle() const noexcept
	{
		return{ center, Max(axes.x, axes.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF SuperEllipse::boundingRect() const noexcept
	{
		return{ (center.x - axes.x), (center.y - axes.y), (axes.x * 2), (axes.y * 2) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr SuperEllipse SuperEllipse::lerp(const SuperEllipse& other, const double f) const noexcept
	{
		return{ center.lerp(other.center, f), axes.lerp(other.axes, f), (n + (other.n - n) * f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 SuperEllipse::hash() const noexcept
	{
		return BitwiseHash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceTo
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	double SuperEllipse::distanceTo(const Shape2DType& other) const
	{
		return Geometry2D::Distance(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool SuperEllipse::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersects(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	overlaps
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool SuperEllipse::overlaps(const Shape2DType& other) const
	{
		return Geometry2D::Overlaps(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool SuperEllipse::contains(const Shape2DType& other) const
	{
		return Geometry2D::Contains(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersectsAt
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	Optional<Array<Vec2>> SuperEllipse::intersectsAt(const Shape2DType& other) const
	{
		return Geometry2D::IntersectsAt(*this, other);
	}
}

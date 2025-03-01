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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse::Ellipse(const value_type r) noexcept
		: center{ 0, 0 }
		, axes{ r, r } {}

	constexpr Ellipse::Ellipse(const Concept::Arithmetic auto r) noexcept
		: center{ 0, 0 }
		, axes{ static_cast<value_type>(r), static_cast<value_type>(r) } {}

	constexpr Ellipse::Ellipse(const value_type _a, const value_type _b) noexcept
		: center{ 0, 0 }
		, axes{ _a, _b } {}

	constexpr Ellipse::Ellipse(const Concept::Arithmetic auto _a, const Concept::Arithmetic auto _b) noexcept
		: center{ 0, 0 }
		, axes{ static_cast<value_type>(_a), static_cast<value_type>(_b) } {}

	constexpr Ellipse::Ellipse(const value_type _x, const value_type _y, const value_type r) noexcept
		: center{ _x, _y }
		, axes{ r, r } {}

	constexpr Ellipse::Ellipse(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto r) noexcept
		: center{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, axes{ static_cast<value_type>(r), static_cast<value_type>(r) } {}

	constexpr Ellipse::Ellipse(const value_type _x, const value_type _y, const value_type _a, const value_type _b) noexcept
		: center{ _x, _y }
		, axes{ _a, _b } {}

	constexpr Ellipse::Ellipse(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _a, const Concept::Arithmetic auto _b) noexcept
		: center{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, axes{ static_cast<value_type>(_a), static_cast<value_type>(_b) } {}

	constexpr Ellipse::Ellipse(const position_type& _center, const value_type r) noexcept
		: center{ _center }
		, axes{ r, r } {}

	constexpr Ellipse::Ellipse(const position_type& _center, const Concept::Arithmetic auto r) noexcept
		: center{ _center }
		, axes{ static_cast<value_type>(r), static_cast<value_type>(r) } {}

	constexpr Ellipse::Ellipse(const position_type& _center, const value_type _a, const value_type _b) noexcept
		: center{ _center }
		, axes{ _a, _b } {}

	constexpr Ellipse::Ellipse(const position_type& _center, const Concept::Arithmetic auto _a, const Concept::Arithmetic auto _b) noexcept
		: center{ _center }
		, axes{ static_cast<value_type>(_a), static_cast<value_type>(_b) } {}

	constexpr Ellipse::Ellipse(const value_type _x, const value_type _y, const size_type& _axes) noexcept
		: center{ _x, _y }
		, axes{ _axes } {}

	constexpr Ellipse::Ellipse(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const size_type& _axes) noexcept
		: center{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, axes{ _axes } {}

	constexpr Ellipse::Ellipse(const position_type& _center, const size_type& _axes) noexcept
		: center{ _center }
		, axes{ _axes } {}

	constexpr Ellipse::Ellipse(const Circle& circle) noexcept
		: center{ circle.center }
		, axes{ circle.r, circle.r } {}

	constexpr Ellipse::Ellipse(const RectF& rect) noexcept
		: center{ rect.center() }
		, axes{ (rect.size.x * 0.5), (rect.size.y * 0.5) } {}

	////////////////////////////////////////////////////////////////
	//
	//	withX
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse Ellipse::withX(const value_type _x) const noexcept
	{
		return{ _x, center.y, axes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withY
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse Ellipse::withY(const value_type _y) const noexcept
	{
		return{ center.x, _y, axes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse Ellipse::withCenter(const value_type _x, const value_type _y) const noexcept
	{
		return{ _x, _y, axes };
	}

	constexpr Ellipse Ellipse::withCenter(const position_type _center) const noexcept
	{
		return{ _center, axes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withA
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse Ellipse::withA(const value_type _a) const noexcept
	{
		return{ center, _a, axes.y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withB
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse Ellipse::withB(const value_type _b) const noexcept
	{
		return{ center, axes.x, _b };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withAxes
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse Ellipse::withAxes(const value_type _a, const value_type _b) const noexcept
	{
		return{ center, _a, _b };
	}

	constexpr Ellipse Ellipse::withAxes(const size_type _axes) const noexcept
	{
		return{ center, _axes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse& Ellipse::setPos(const value_type _x, const value_type _y) noexcept
	{
		center.set(_x, _y);
		return *this;
	}

	constexpr Ellipse& Ellipse::setPos(const position_type _center) noexcept
	{
		center = _center;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse& Ellipse::setCenter(const value_type _x, const value_type _y) noexcept
	{
		center.set(_x, _y);
		return *this;
	}

	constexpr Ellipse& Ellipse::setCenter(const position_type _center) noexcept
	{
		center = _center;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setA
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse& Ellipse::setA(const value_type _a) noexcept
	{
		axes.x = _a;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setB
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse& Ellipse::setB(const value_type _b) noexcept
	{
		axes.y = _b;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setAxes
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse& Ellipse::setAxes(const value_type _a, const value_type _b) noexcept
	{
		axes.set(_a, _b);
		return *this;
	}

	constexpr Ellipse& Ellipse::setAxes(const size_type _axes) noexcept
	{
		axes = _axes;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse& Ellipse::set(const value_type _x, const value_type _y, const value_type _r) noexcept
	{
		center.set(_x, _y);
		axes.set(_r, _r);
		return *this;
	}

	constexpr Ellipse& Ellipse::set(const position_type& _center, const value_type _r) noexcept
	{
		center = _center;
		axes.set(_r, _r);
		return *this;
	}

	constexpr Ellipse& Ellipse::set(const value_type _x, const value_type _y, const value_type _a, const value_type _b) noexcept
	{
		center.set(_x, _y);
		axes.set(_a, _b);
		return *this;
	}

	constexpr Ellipse& Ellipse::set(const position_type& _center, const value_type _a, const value_type _b) noexcept
	{
		center = _center;
		axes.set(_a, _b);
		return *this;
	}

	constexpr Ellipse& Ellipse::set(const value_type _x, const value_type _y, const size_type& _axes) noexcept
	{
		center.set(_x, _y);
		axes = _axes;
		return *this;
	}

	constexpr Ellipse& Ellipse::set(const position_type& _center, const size_type& _axes) noexcept
	{
		center = _center;
		axes = _axes;
		return *this;
	}

	constexpr Ellipse& Ellipse::set(const Circle& circle) noexcept
	{
		center = circle.center;
		axes.set(circle.r, circle.r);
		return *this;
	}

	constexpr Ellipse& Ellipse::set(const Ellipse& ellipse) noexcept
	{
		center = ellipse.center;
		axes = ellipse.axes;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse Ellipse::movedBy(const value_type _x, const value_type _y) const noexcept
	{
		return{ center.movedBy(_x, _y), axes };
	}

	constexpr Ellipse Ellipse::movedBy(const position_type v) const noexcept
	{
		return{ center.movedBy(v), axes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse& Ellipse::moveBy(const value_type _x, const value_type _y) noexcept
	{
		center.moveBy(_x, _y);
		return *this;
	}

	constexpr Ellipse& Ellipse::moveBy(const position_type v) noexcept
	{
		center.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	stretched
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse Ellipse::stretched(const value_type size) const noexcept
	{
		return{ center, (axes.x + size), (axes.y + size) };
	}

	constexpr Ellipse Ellipse::stretched(const double _x, const double _y) const noexcept
	{
		return{ center, (axes.x + _x), (axes.y + _y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaled
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse Ellipse::scaled(const double s) const noexcept
	{
		return{ center, (axes.x * s), (axes.y * s) };
	}

	constexpr Ellipse Ellipse::scaled(const double sx, const double sy) const noexcept
	{
		return{ center, (axes.x * sx), (axes.y * sy) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	top, right, bottom, left
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse::position_type Ellipse::top() const noexcept
	{
		return{ center.x, (center.y - axes.y) };
	}

	constexpr Ellipse::position_type Ellipse::right() const noexcept
	{
		return{ (center.x + axes.x), center.y };
	}

	constexpr Ellipse::position_type Ellipse::bottom() const noexcept
	{
		return{ center.x, (center.y + axes.y) };
	}

	constexpr Ellipse::position_type Ellipse::left() const noexcept
	{
		return{ (center.x - axes.x), center.y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	horizontalDiameter, verticalDiameter
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Ellipse::horizontalDiameter() const noexcept
	{
		return{ left(), right() };
	}

	constexpr Line Ellipse::verticalDiameter() const noexcept
	{
		return{ top(), bottom() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse::value_type Ellipse::area() const noexcept
	{
		return (Math::Pi * axes.x * axes.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	inline Ellipse::value_type Ellipse::perimeter() const noexcept
	{
		if ((axes.x + axes.y) == 0)
		{
			return 0.0;
		}

		if (axes.x == axes.y)
		{
			return (Math::TwoPi * axes.x);
		}

		// Ramanujan's second approximation
		const value_type _a = axes.x;
		const value_type _b = axes.y;
		const value_type h = ((_a - _b) * (_a - _b)) / ((_a + _b) * (_a + _b));
		return Math::Pi * (_a + _b) * (1 + ((3 * h) / (10 + std::sqrt(4 - 3 * h))));
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingCircle
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Ellipse::boundingCircle() const noexcept
	{
		return{ center, Max(axes.x, axes.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF Ellipse::boundingRect() const noexcept
	{
		return{ (center.x - axes.x), (center.y - axes.y), (axes.x * 2), (axes.y * 2) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr Ellipse Ellipse::lerp(const Ellipse& other, const double f) const noexcept
	{
		return{ center.lerp(other.center, f), axes.lerp(other.axes, f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Ellipse::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	inline constexpr bool Ellipse::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersect(*this, other);
	}
}

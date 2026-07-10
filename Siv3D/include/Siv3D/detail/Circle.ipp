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
	namespace detail
	{
		[[nodiscard]]
		constexpr Circle::position_type FromCircleAnchor(const Anchor anchor, Circle::value_type x, Circle::value_type y, const Circle::value_type r) noexcept
		{
			if (anchor.isLeft())
			{
				x += r;
			}
			else if (anchor.isRight())
			{
				x -= r;
			}

			if (anchor.isTop())
			{
				y += r;
			}
			else if (anchor.isBottom())
			{
				y -= r;
			}

			return{ x, y };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle::Circle(const size_type _r) noexcept
		: center{ 0.0, 0.0 }
		, r{ _r } {}

	constexpr Circle::Circle(const Concept::Arithmetic auto _r) noexcept
		: center{ 0.0, 0.0 }
		, r{ static_cast<value_type>(_r) } {}

	constexpr Circle::Circle(const value_type _x, const value_type _y, const size_type _r) noexcept
		: center{ _x, _y }
		, r{ _r } {}

	constexpr Circle::Circle(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _r) noexcept
		: center{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const position_type _center, const size_type _r) noexcept
		: center{ _center }
		, r{ _r } {}

	constexpr Circle::Circle(const position_type _center, const Concept::Arithmetic auto _r) noexcept
		: center{ _center }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Anchor anchor, const value_type _x, const value_type _y, const size_type _r) noexcept
		: center{ detail::FromCircleAnchor(anchor, _x, _y, _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Anchor anchor, const value_type _x, const value_type _y, const Concept::Arithmetic auto _r) noexcept
		: Circle{ anchor, _x, _y, static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Anchor anchor, const position_type _pos, const size_type _r) noexcept
		: Circle{ anchor, _pos.x, _pos.y, _r } {}

	constexpr Circle::Circle(const Anchor anchor, const position_type _pos, const Concept::Arithmetic auto _r) noexcept
		: Circle{ anchor, _pos.x, _pos.y, static_cast<size_type>(_r) } {}

	inline Circle::Circle(const position_type p0, const position_type p1) noexcept
		: center{ (p0 + p1) / 2.0 }
		, r{ p0.distanceFrom(p1) / 2.0 } {}

	inline Circle::Circle(const Line& diameter) noexcept
		: Circle{ diameter.start, diameter.end } {}

	inline Circle::Circle(const Arg::center_<position_type> _center, const position_type& p) noexcept
		: center{ *_center }
		, r{ p.distanceFrom(*_center) } {}

	////////////////////////////////////////////////////////////////
	//
	//	withX
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Circle::withX(const value_type _x) const noexcept
	{
		return{ _x, center.y, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withY
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Circle::withY(const value_type _y) const noexcept
	{
		return{ center.x, _y, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Circle::withCenter(const value_type _x, const value_type _y) const noexcept
	{
		return{ _x, _y, r };
	}

	constexpr Circle Circle::withCenter(const position_type _center) const noexcept
	{
		return{ _center, r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withR
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Circle::withR(const value_type _r) const noexcept
	{
		return{ center, _r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle& Circle::setPos(const value_type _x, const value_type _y) noexcept
	{
		center.set(_x, _y);
		return *this;
	}

	constexpr Circle& Circle::setPos(const position_type _center) noexcept
	{
		center = _center;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle& Circle::setCenter(const value_type _x, const value_type _y) noexcept
	{
		center.set(_x, _y);
		return *this;
	}

	constexpr Circle& Circle::setCenter(const position_type _center) noexcept
	{
		center = _center;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setR
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle& Circle::setR(const value_type _r) noexcept
	{
		r = _r;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle& Circle::set(const value_type _x, const value_type _y, const size_type _r) noexcept
	{
		center.set(_x, _y);
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const position_type _center, const size_type _r) noexcept
	{
		center = _center;
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const Anchor anchor, const value_type _x, const value_type _y, const size_type _r) noexcept
	{
		center = detail::FromCircleAnchor(anchor, _x, _y, _r);
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const Anchor anchor, const position_type _pos, const size_type _r) noexcept
	{
		return set(anchor, _pos.x, _pos.y, _r);
	}

	constexpr Circle& Circle::set(const Circle& circle) noexcept
	{
		center = circle.center;
		r = circle.r;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Circle::movedBy(const value_type _x, const value_type _y) const noexcept
	{
		return{ center.movedBy(_x, _y), r };
	}

	constexpr Circle Circle::movedBy(const position_type v) const noexcept
	{
		return{ center.movedBy(v), r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle& Circle::moveBy(const value_type _x, const value_type _y) noexcept
	{
		center.moveBy(_x, _y);
		return *this;
	}

	constexpr Circle& Circle::moveBy(const position_type v) noexcept
	{
		center.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffset
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Circle::withOffset(const value_type _x, const value_type _y) const noexcept
	{
		return{ center.withOffset(_x, _y), r };
	}

	constexpr Circle Circle::withOffset(const position_type v) const noexcept
	{
		return{ center.withOffset(v), r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withOffsetX, withOffsetY
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Circle::withOffsetX(const value_type _x) const noexcept
	{
		return{ (center.x + _x), center.y, r };
	}

	constexpr Circle Circle::withOffsetY(const value_type _y) const noexcept
	{
		return{ center.x, (center.y + _y), r };
	}

	////////////////////////////////////////////////////////////////
	//
	//	stretched
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Circle::stretched(const value_type size) const noexcept
	{
		return{ center, (r + size) };
	}

	constexpr Ellipse Circle::stretched(const double _x, const double _y) const noexcept
	{
		return{ center, (r + _x), (r + _y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaled
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Circle::scaled(const double s) const noexcept
	{
		return{ center, (r * s) };
	}

	constexpr Ellipse Circle::scaled(const double sx, const double sy) const noexcept
	{
		return{ center, (r * sx), (r * sy) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Circle::isEmpty() const noexcept
	{
		return (r == 0.0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasArea
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Circle::hasArea() const noexcept
	{
		return (r != 0.0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	top, right, bottom, left
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle::position_type Circle::top() const noexcept
	{
		return{ center.x, (center.y - r) };
	}

	constexpr Circle::position_type Circle::right() const noexcept
	{
		return{ (center.x + r), center.y };
	}

	constexpr Circle::position_type Circle::bottom() const noexcept
	{
		return{ center.x, (center.y + r) };
	}

	constexpr Circle::position_type Circle::left() const noexcept
	{
		return{ (center.x - r), center.y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	horizontalDiameter, verticalDiameter
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Circle::horizontalDiameter() const noexcept
	{
		return{ left(), right() };
	}

	constexpr Line Circle::verticalDiameter() const noexcept
	{
		return{ top(), bottom() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle::value_type Circle::area() const noexcept
	{
		return (r * r * Math::Pi);
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle::value_type Circle::perimeter() const noexcept
	{
		return (r * Math::TwoPi);
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF Circle::boundingRect() const noexcept
	{
		return{ Arg::center(center), (r * 2) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr Circle Circle::lerp(const Circle& other, const double f) const noexcept
	{
		return{ center.lerp(other.center, f), (r + (other.r - r) * f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Circle::hash() const noexcept
	{
		return BitwiseHash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool Circle::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersects(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	overlaps
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool Circle::overlaps(const Shape2DType& other) const
	{
		return Geometry2D::Overlaps(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	contains
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool Circle::contains(const Shape2DType& other) const
	{
		return Geometry2D::Contains(*this, other);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersectsAt
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	Optional<Array<Vec2>> Circle::intersectsAt(const Shape2DType& other) const
	{
		return Geometry2D::IntersectsAt(*this, other);
	}
}

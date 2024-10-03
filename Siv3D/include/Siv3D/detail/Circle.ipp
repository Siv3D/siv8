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

namespace s3d
{
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

	constexpr Circle::Circle(const Arg::center_<position_type> _center, const size_type _r) noexcept
		: center{ *_center }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::center_<position_type> _center, const Concept::Arithmetic auto _r) noexcept
		: center{ *_center }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::topLeft_<position_type> _topLeft, const size_type _r) noexcept
		: center{ (_topLeft->x + _r), (_topLeft->y + _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::topLeft_<position_type> _topLeft, const Concept::Arithmetic auto _r) noexcept
		: center{ (_topLeft->x + static_cast<value_type>(_r)), (_topLeft->y + static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::topCenter_<position_type> _topCenter, const size_type _r) noexcept
		: center{ _topCenter->x, (_topCenter->y + _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::topCenter_<position_type> _topCenter, const Concept::Arithmetic auto _r) noexcept
		: center{ _topCenter->x, (_topCenter->y + static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::topRight_<position_type> _topRight, const size_type _r) noexcept
		: center{ (_topRight->x - _r), (_topRight->y + _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::topRight_<position_type> _topRight, const Concept::Arithmetic auto _r) noexcept
		: center{ (_topRight->x - static_cast<value_type>(_r)), (_topRight->y + static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::rightCenter_<position_type> _rightCenter, const size_type _r) noexcept
		: center{ (_rightCenter->x - _r), _rightCenter->y }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::rightCenter_<position_type> _rightCenter, const Concept::Arithmetic auto _r) noexcept
		: center{ (_rightCenter->x - static_cast<value_type>(_r)), _rightCenter->y }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::bottomRight_<position_type> _bottomRight, const size_type _r) noexcept
		: center{ (_bottomRight->x - _r), (_bottomRight->y - _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::bottomRight_<position_type> _bottomRight, const Concept::Arithmetic auto _r) noexcept
		: center{ (_bottomRight->x - static_cast<value_type>(_r)), (_bottomRight->y - static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::bottomCenter_<position_type> _bottomCenter, const size_type _r) noexcept
		: center{ _bottomCenter->x, (_bottomCenter->y - _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::bottomCenter_<position_type> _bottomCenter, const Concept::Arithmetic auto _r) noexcept
		: center{ _bottomCenter->x, (_bottomCenter->y - static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::bottomLeft_<position_type> _bottomLeft, const size_type _r) noexcept
		: center{ (_bottomLeft->x + _r), (_bottomLeft->y - _r) }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::bottomLeft_<position_type> _bottomLeft, const Concept::Arithmetic auto _r) noexcept
		: center{ (_bottomLeft->x + static_cast<value_type>(_r)), (_bottomLeft->y - static_cast<value_type>(_r)) }
		, r{ static_cast<size_type>(_r) } {}

	constexpr Circle::Circle(const Arg::leftCenter_<position_type> _leftCenter, const size_type _r) noexcept
		: center{ (_leftCenter->x + _r), _leftCenter->y }
		, r{ _r } {}

	constexpr Circle::Circle(const Arg::leftCenter_<position_type> _leftCenter, const Concept::Arithmetic auto _r) noexcept
		: center{ (_leftCenter->x + static_cast<value_type>(_r)), _leftCenter->y }
		, r{ static_cast<size_type>(_r) } {}

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

	constexpr Circle& Circle::set(const Arg::center_<position_type> _center, const size_type _r) noexcept
	{
		center = *_center;
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const Arg::topLeft_<position_type> topLeft, const size_type _r) noexcept
	{
		center.set((topLeft->x + _r), (topLeft->y + _r));
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const Arg::topCenter_<position_type> topCenter, const size_type _r) noexcept
	{
		center.set(topCenter->x, (topCenter->y + _r));
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const Arg::topRight_<position_type> topRight, const size_type _r) noexcept
	{
		center.set((topRight->x - _r), (topRight->y + _r));
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const Arg::rightCenter_<position_type> rightCenter, const size_type _r) noexcept
	{
		center.set((rightCenter->x - _r), rightCenter->y);
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const Arg::bottomRight_<position_type> bottomRight, const size_type _r) noexcept
	{
		center.set((bottomRight->x - _r), (bottomRight->y - _r));
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const Arg::bottomCenter_<position_type> bottomCenter, const size_type _r) noexcept
	{
		center.set(bottomCenter->x, (bottomCenter->y - _r));
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const Arg::bottomLeft_<position_type> bottomLeft, const size_type _r) noexcept
	{
		center.set((bottomLeft->x + _r), (bottomLeft->y - _r));
		r = _r;
		return *this;
	}

	constexpr Circle& Circle::set(const Arg::leftCenter_<position_type> leftCenter, const size_type _r) noexcept
	{
		center.set((leftCenter->x + _r), leftCenter->y);
		r = _r;
		return *this;
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
		return (2 * r * Math::Pi);
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
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	inline constexpr bool Circle::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersect(*this, other);
	}
}

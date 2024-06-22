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

	constexpr Rect::Rect(const value_type _size) noexcept
		: pos{ 0, 0 }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Concept::Integral auto _size) noexcept
		: pos{ 0, 0 }
		, size{ static_cast<value_type>(_size), static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const value_type _w, const value_type _h) noexcept
		: pos{ 0, 0 }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: pos{ 0, 0 }
		, size{ static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const size_type _size) noexcept
		: pos{ 0, 0 }
		, size{ _size } {}

	constexpr Rect::Rect(const value_type _x, const value_type _y, const value_type _size) noexcept
		: pos{ _x, _y }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Concept::Integral auto _x, const Concept::Integral auto _y, const Concept::Integral auto _size) noexcept
		: pos{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, size{ static_cast<value_type>(_size), static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const value_type _x, const value_type _y, const value_type _w, const value_type _h) noexcept
		: pos{ _x, _y }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Concept::Integral auto _x, const Concept::Integral auto _y, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: pos{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, size{ static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const value_type _x, const value_type _y, const size_type _size) noexcept
		: pos{ _x, _y }
		, size{ _size } {}

	constexpr Rect::Rect(const Concept::Integral auto _x, const Concept::Integral auto _y, const size_type _size) noexcept
		: pos{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, size{ _size } {}

	constexpr Rect::Rect(const position_type _pos, const value_type _size) noexcept
		: pos{ _pos }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const position_type _pos, const Concept::Integral auto _size) noexcept
		: pos{ _pos }
		, size{ static_cast<value_type>(_size), static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const position_type _pos, const value_type _w, const value_type _h) noexcept
		: pos{ _pos }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const position_type _pos, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: pos{ _pos }
		, size{ static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const position_type _pos, const size_type _size) noexcept
		: pos{ _pos }
		, size{ _size } {}

	constexpr Rect::Rect(const Arg::center_<position_type> _center, const value_type _size) noexcept
		: pos{ (_center->x - _size / 2), (_center->y - _size / 2) }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Arg::center_<position_type> _center, const Concept::Integral auto _size) noexcept
		: Rect{ _center, static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const Arg::center_<position_type> _center, const value_type _w, const value_type _h) noexcept
		: pos{ (_center->x - _w / 2), (_center->y - _h / 2) }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Arg::center_<position_type> _center, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: Rect{ _center, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const Arg::center_<position_type> _center, const size_type _size) noexcept
		: pos{ (_center->x - _size.x / 2), (_center->y - _size.y / 2) }
		, size{ _size.x, _size.y } {}

	constexpr Rect::Rect(const Arg::topLeft_<position_type> topLeft, const value_type _size) noexcept
		: pos{ *topLeft }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Arg::topLeft_<position_type> topLeft, const Concept::Integral auto _size) noexcept
		: Rect{ topLeft, static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const Arg::topLeft_<position_type> topLeft, const value_type _w, const value_type _h) noexcept
		: pos{ *topLeft }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Arg::topLeft_<position_type> topLeft, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: Rect{ topLeft, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const Arg::topLeft_<position_type> topLeft, const size_type _size) noexcept
		: pos{ *topLeft }
		, size{ _size.x, _size.y } {}

	constexpr Rect::Rect(const Arg::topCenter_<position_type> topCenter, const value_type _size) noexcept
		: pos{ (topCenter->x - _size / 2), topCenter->y }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Arg::topCenter_<position_type> topCenter, const Concept::Integral auto _size) noexcept
		: Rect{ topCenter, static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const Arg::topCenter_<position_type> topCenter, const value_type _w, const value_type _h) noexcept
		: pos{ (topCenter->x - _w / 2), topCenter->y }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Arg::topCenter_<position_type> topCenter, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		:Rect{ topCenter, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const Arg::topCenter_<position_type> topCenter, const size_type _size) noexcept
		: pos{ (topCenter->x - _size.x / 2), topCenter->y }
		, size{ _size.x, _size.y } {}

	constexpr Rect::Rect(const Arg::topRight_<position_type> topRight, const value_type _size) noexcept
		: pos{ (topRight->x - _size), topRight->y }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Arg::topRight_<position_type> topRight, const Concept::Integral auto _size) noexcept
		: Rect{ topRight, static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const Arg::topRight_<position_type> topRight, const value_type _w, const value_type _h) noexcept
		: pos{ (topRight->x - _w), topRight->y }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Arg::topRight_<position_type> topRight, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: Rect{ topRight, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const Arg::topRight_<position_type> topRight, const size_type _size) noexcept
		: pos{ (topRight->x - _size.x), topRight->y }
		, size{ _size.x, _size.y } {}

	constexpr Rect::Rect(const Arg::rightCenter_<position_type> rightCenter, const value_type _size) noexcept
		: pos{ (rightCenter->x - _size), (rightCenter->y - _size / 2) }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Arg::rightCenter_<position_type> rightCenter, const Concept::Integral auto _size) noexcept
		: Rect{ rightCenter, static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const Arg::rightCenter_<position_type> rightCenter, const value_type _w, const value_type _h) noexcept
		: pos{ (rightCenter->x - _w), (rightCenter->y - _h / 2) }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Arg::rightCenter_<position_type> rightCenter, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: Rect{ rightCenter, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const Arg::rightCenter_<position_type> rightCenter, const size_type _size) noexcept
		: pos{ (rightCenter->x - _size.x), (rightCenter->y - _size.y / 2) }
		, size{ _size.x, _size.y } {}

	constexpr Rect::Rect(const Arg::bottomRight_<position_type> bottomRight, const value_type _size) noexcept
		: pos{ (bottomRight->x - _size), (bottomRight->y - _size) }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Arg::bottomRight_<position_type> bottomRight, const Concept::Integral auto _size) noexcept
		: Rect{ bottomRight, static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const Arg::bottomRight_<position_type> bottomRight, const value_type _w, const value_type _h) noexcept
		: pos{ (bottomRight->x - _w), (bottomRight->y - _h) }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Arg::bottomRight_<position_type> bottomRight, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: Rect{ bottomRight, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const Arg::bottomRight_<position_type> bottomRight, const size_type _size) noexcept
		: pos{ (bottomRight->x - _size.x), (bottomRight->y - _size.y) }
		, size{ _size.x, _size.y } {}

	constexpr Rect::Rect(const Arg::bottomCenter_<position_type> bottomCenter, const value_type _size) noexcept
		: pos{ (bottomCenter->x - _size / 2), (bottomCenter->y - _size) }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Arg::bottomCenter_<position_type> bottomCenter, const Concept::Integral auto _size) noexcept
		: Rect{ bottomCenter, static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const Arg::bottomCenter_<position_type> bottomCenter, const value_type _w, const value_type _h) noexcept
		: pos{ (bottomCenter->x - _w / 2), (bottomCenter->y - _h) }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Arg::bottomCenter_<position_type> bottomCenter, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: Rect{ bottomCenter, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const Arg::bottomCenter_<position_type> bottomCenter, const size_type _size) noexcept
		: pos{ (bottomCenter->x - _size.x / 2), (bottomCenter->y - _size.y) }
		, size{ _size.x, _size.y } {}

	constexpr Rect::Rect(const Arg::bottomLeft_<position_type> bottomLeft, const value_type _size) noexcept
		: pos{ bottomLeft->x, (bottomLeft->y - _size) }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Arg::bottomLeft_<position_type> bottomLeft, const Concept::Integral auto _size) noexcept
		: Rect{ bottomLeft, static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const Arg::bottomLeft_<position_type> bottomLeft, const value_type _w, const value_type _h) noexcept
		: pos{ bottomLeft->x, (bottomLeft->y - _h) }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Arg::bottomLeft_<position_type> bottomLeft, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: Rect{ bottomLeft, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const Arg::bottomLeft_<position_type> bottomLeft, const size_type _size) noexcept
		: pos{ bottomLeft->x, (bottomLeft->y - _size.y) }
		, size{ _size.x, _size.y } {}

	constexpr Rect::Rect(const Arg::leftCenter_<position_type> leftCenter, const value_type _size) noexcept
		: pos{ leftCenter->x, (leftCenter->y - _size / 2) }
		, size{ _size, _size } {}

	constexpr Rect::Rect(const Arg::leftCenter_<position_type> leftCenter, const Concept::Integral auto _size) noexcept
		: Rect{ leftCenter, static_cast<value_type>(_size) } {}

	constexpr Rect::Rect(const Arg::leftCenter_<position_type> leftCenter, const value_type _w, const value_type _h) noexcept
		: pos{ leftCenter->x, (leftCenter->y - _h / 2) }
		, size{ _w, _h } {}

	constexpr Rect::Rect(const Arg::leftCenter_<position_type> leftCenter, const Concept::Integral auto _w, const Concept::Integral auto _h) noexcept
		: Rect{ leftCenter, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr Rect::Rect(const Arg::leftCenter_<position_type> leftCenter, const size_type _size) noexcept
		: pos{ leftCenter->x, (leftCenter->y - _size.y / 2) }
		, size{ _size.x, _size.y } {}

	////////////////////////////////////////////////////////////////
	//
	//	withX
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withX(const value_type _x) const noexcept
	{
		return{ _x, pos.y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withY
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withY(const value_type _y) const noexcept
	{
		return{ pos.x, _y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withPos
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withPos(const value_type _x, const value_type _y) const noexcept
	{
		return{ _x, _y, size };
	}

	constexpr Rect Rect::withPos(const position_type _pos) const noexcept
	{
		return{ _pos, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withW
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withW(const value_type _w) const noexcept
	{
		return{ pos, _w, size.y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withH
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withH(const value_type _h) const noexcept
	{
		return{ pos, size.x, _h };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withSize
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withSize(const value_type _w, const value_type _h) const noexcept
	{
		return{ pos, _w, _h };
	}

	constexpr Rect Rect::withSize(const size_type _size) const noexcept
	{
		return{ pos, _size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withCenter(const value_type _cx, const value_type _cy) const noexcept
	{
		return{ (_cx - size.x / 2), (_cy - size.y / 2), size };
	}

	constexpr Rect Rect::withCenter(const position_type _center) const noexcept
	{
		return{ (_center.x - size.x / 2), (_center.y - size.y / 2), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect& Rect::setPos(const value_type _x, const value_type _y) noexcept
	{
		pos.set(_x, _y);
		return *this;
	}

	constexpr Rect& Rect::setPos(const position_type _pos) noexcept
	{
		pos = _pos;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect& Rect::setCenter(const value_type _x, const value_type _y) noexcept
	{
		pos.set((_x - size.x / 2), (_y - size.y / 2));
		return *this;
	}

	constexpr Rect& Rect::setCenter(const position_type _pos) noexcept
	{
		pos.set((_pos.x - size.x / 2), (_pos.y - size.y / 2));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSize
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect& Rect::setSize(const value_type _size) noexcept
	{
		size.set(_size, _size);
		return *this;
	}

	constexpr Rect& Rect::setSize(const value_type _w, const value_type _h) noexcept
	{
		size.set(_w, _h);
		return *this;
	}

	constexpr Rect& Rect::setSize(const size_type _size) noexcept
	{
		size = _size;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect& Rect::set(const value_type _x, const value_type _y, const value_type _w, const value_type _h) noexcept
	{
		pos.set(_x, _y);
		size.set(_w, _h);
		return *this;
	}

	constexpr Rect& Rect::set(const value_type _x, const value_type _y, const value_type _size) noexcept
	{
		pos.set(_x, _y);
		size.set(_size, _size);
		return *this;
	}

	constexpr Rect& Rect::set(const value_type _x, const value_type _y, const size_type _size) noexcept
	{
		pos.set(_x, _y);
		size = _size;
		return *this;
	}

	constexpr Rect& Rect::set(const position_type _pos, const value_type _size) noexcept
	{
		pos = _pos;
		size.set(_size, _size);
		return *this;
	}

	constexpr Rect& Rect::set(const position_type _pos, const value_type _w, const value_type _h) noexcept
	{
		pos = _pos;
		size.set(_w, _h);
		return *this;
	}

	constexpr Rect& Rect::set(const position_type _pos, const size_type _size) noexcept
	{
		pos = _pos;
		size = _size;
		return *this;
	}

	constexpr Rect& Rect::set(const Rect& r) noexcept
	{
		pos = r.pos;
		size = r.size;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::movedBy(const value_type _x, const value_type _y) const noexcept
	{
		return{ pos.movedBy(_x, _y), size };
	}

	constexpr Rect Rect::movedBy(const size_type v) const noexcept
	{
		return{ pos.movedBy(v), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect& Rect::moveBy(const value_type _x, const value_type _y) noexcept
	{
		pos.moveBy(_x, _y);
		return *this;
	}

	constexpr Rect& Rect::moveBy(const size_type v) noexcept
	{
		pos.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	stretched
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::stretched(const value_type xy) const noexcept
	{
		return stretched({ xy, xy });
	}

	constexpr Rect Rect::stretched(const value_type _x, const value_type _y) const noexcept
	{
		return stretched({ _x, _y });
	}

	constexpr Rect Rect::stretched(const size_type xy) const noexcept
	{
		return{ (pos - xy), (size + xy * 2) };
	}

	constexpr Rect Rect::stretched(const value_type top, const value_type right, const value_type bottom, const value_type left) const noexcept
	{
		return{ (pos.x - left), (pos.y - top), (size.x + left + right), (size.y + top + bottom) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF Rect::scaledFromCenter(const double s) const noexcept
	{
		return{ Arg::center = center(), (size * s) };
	}

	constexpr RectF Rect::scaledFromCenter(const double sx, const double sy) const noexcept
	{
		return{ Arg::center = center(), (size.x * sx), (size.y * sy) };
	}

	constexpr RectF Rect::scaledFromCenter(const Vec2 s) const noexcept
	{
		return{ Arg::center = center(), (size * s) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromTopLeft
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF Rect::scaledFromTopLeft(const double s) const noexcept
	{
		return{ pos, (size * s) };
	}

	constexpr RectF Rect::scaledFromTopLeft(const double sx, const double sy) const noexcept
	{
		return{ pos, (size.x * sx), (size.y * sy) };
	}

	constexpr RectF Rect::scaledFromTopLeft(const Vec2 s) const noexcept
	{
		return{ pos, (size * s) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF Rect::scaledFrom(const Vec2 _pos, const double s) const noexcept
	{
		return{ (_pos.x + (pos.x - _pos.x) * s), (_pos.y + (pos.y - _pos.y) * s), (size * s) };
	}

	constexpr RectF Rect::scaledFrom(const Vec2 _pos, const double sx, const double sy) const noexcept
	{
		return{ (_pos.x + (pos.x - _pos.x) * sx), (_pos.y + (pos.y - _pos.y) * sy), (size.x * sx), (size.y * sy) };
	}

	constexpr RectF Rect::scaledFrom(const Vec2 _pos, const Vec2 s) const noexcept
	{
		return{ (_pos.x + (pos.x - _pos.x) * s.x), (_pos.y + (pos.y - _pos.y) * s.y), (size * s) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect::operator bool() const noexcept
	{
		return hasArea();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Rect::isEmpty() const noexcept
	{
		return ((size.x == 0) || (size.y == 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasArea
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Rect::hasArea() const noexcept
	{
		return ((size.x != 0) && (size.y != 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftX, rightX, topY, bottomY
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect::value_type Rect::leftX() const noexcept
	{
		return pos.x;
	}

	constexpr Rect::value_type Rect::rightX() const noexcept
	{
		return (pos.x + size.x);
	}

	constexpr Rect::value_type Rect::topY() const noexcept
	{
		return pos.y;
	}

	constexpr Rect::value_type Rect::bottomY() const noexcept
	{
		return (pos.y + size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	centerX, centerY
	//
	////////////////////////////////////////////////////////////////

	constexpr double Rect::centerX() const noexcept
	{
		return (pos.x + size.x * 0.5);
	}

	constexpr double Rect::centerY() const noexcept
	{
		return (pos.y + size.y * 0.5);
	}

	////////////////////////////////////////////////////////////////
	//
	//	tl, tr, br, bl
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect::size_type Rect::tl() const noexcept
	{
		return pos;
	}

	constexpr Rect::size_type Rect::tr() const noexcept
	{
		return{ (pos.x + size.x), pos.y };
	}

	constexpr Rect::size_type Rect::br() const noexcept
	{
		return{ (pos.x + size.x), (pos.y + size.y) };
	}

	constexpr Rect::size_type Rect::bl() const noexcept
	{
		return{ pos.x, (pos.y + size.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	topCenter, rightCenter, bottomCenter, leftCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 Rect::topCenter() const noexcept
	{
		return{ (pos.x + size.x * 0.5), pos.y };
	}

	constexpr Vec2 Rect::rightCenter() const noexcept
	{
		return{ (pos.x + size.x), (pos.y + size.y * 0.5) };
	}

	constexpr Vec2 Rect::bottomCenter() const noexcept
	{
		return{ (pos.x + size.x * 0.5), (pos.y + size.y) };
	}

	constexpr Vec2 Rect::leftCenter() const noexcept
	{
		return{ pos.x, (pos.y + size.y * 0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	center
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 Rect::center() const noexcept
	{
		return{ (pos.x + size.x * 0.5), (pos.y + size.y * 0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getRelativePoint
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 Rect::getRelativePoint(const double relativeX, const double relativeY) const noexcept
	{
		return{ (pos.x + size.x * relativeX), (pos.y + size.y * relativeY) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	top, right, bottom, left
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Rect::top() const noexcept
	{
		return{ tl(), tr() };
	}

	constexpr Line Rect::right() const noexcept
	{
		return{ tr(), br() };
	}

	constexpr Line Rect::bottom() const noexcept
	{
		return{ br(), bl() };
	}

	constexpr Line Rect::left() const noexcept
	{
		return{ bl(), tl() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtIndex
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect::position_type Rect::pointAtIndex(const size_t index) const
	{
		if (index == 0)
		{
			return tl();
		}
		else if (index == 1)
		{
			return tr();
		}
		else if (index == 2)
		{
			return br();
		}
		else if (index == 3)
		{
			return bl();
		}
		else
		{
			ThrowPointAtIndexOutOfRange();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	sideAtIndex
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Rect::sideAtIndex(const size_t index) const
	{
		if (index == 0)
		{
			return top();
		}
		else if (index == 1)
		{
			return right();
		}
		else if (index == 2)
		{
			return bottom();
		}
		else if (index == 3)
		{
			return left();
		}
		else
		{
			ThrowSideAtIndexOutOfRange();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	triangleAtIndex
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle Rect::triangleAtIndex(const size_t index) const
	{
		if (index == 0)
		{
			return{ pos, tr(), bl() };
		}
		else if (index == 1)
		{
			return{ bl(), tr(), br() };
		}
		else
		{
			ThrowTriangleAtIndexOutOfRange();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect::value_type Rect::area() const noexcept
	{
		return (size.x * size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect::value_type Rect::perimeter() const noexcept
	{
		return ((size.x + size.y) * 2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	horizontalAspectRatio
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Type Rect::horizontalAspectRatio() const noexcept
	{
		if (size.y == 0)
		{
			return 0;
		}

		return (static_cast<Type>(size.x) / size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	copyToTop
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::copyToTop(const value_type distance) const noexcept
	{
		return{ pos.x, (pos.y - size.y - distance), size };
	}

	constexpr RectF Rect::copyToTop(const Concept::FloatingPoint auto distance) const noexcept
	{
		return{ pos.x, (pos.y - size.y - distance), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	copyToRight
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::copyToRight(const value_type distance) const noexcept
	{
		return{ (pos.x + size.x + distance), pos.y, size };
	}

	constexpr RectF Rect::copyToRight(const Concept::FloatingPoint auto distance) const noexcept
	{
		return{ (pos.x + size.x + distance), pos.y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	copyToBottom
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::copyToBottom(const value_type distance) const noexcept
	{
		return{ pos.x, (pos.y + size.y + distance), size };
	}

	constexpr RectF Rect::copyToBottom(const Concept::FloatingPoint auto distance) const noexcept
	{
		return{ pos.x, (pos.y + size.y + distance), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	copyToLeft
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::copyToLeft(const value_type distance) const noexcept
	{
		return{ (pos.x - size.x - distance), pos.y, size };
	}

	constexpr RectF Rect::copyToLeft(const Concept::FloatingPoint auto distance) const noexcept
	{
		return{ (pos.x - size.x - distance), pos.y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	clamped
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::clamped(const Rect& area) const noexcept
	{
		const value_type x0 = Clamp(pos.x, area.pos.x, area.rightX());
		const value_type y0 = Clamp(pos.y, area.pos.y, area.bottomY());
		const value_type x1 = Clamp(rightX(), area.pos.x, area.rightX());
		const value_type y1 = Clamp(bottomY(), area.pos.y, area.bottomY());
		return{ x0, y0, (x1 - x0), (y1 - y0) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Rect::hash() const noexcept
	{
		return Hash(*this);
	}









	////////////////////////////////////////////////////////////////
	//
	//	Empty
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::Empty() noexcept
	{
		return{ 0, 0, 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromPoints
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::FromPoints(const position_type a, const position_type b) noexcept
	{
		const auto [x0, x1] = MinMax(a.x, b.x);
		const auto [y0, y1] = MinMax(a.y, b.y);
		return{ x0, y0, (x1 - x0), (y1- y0) };
	}
}

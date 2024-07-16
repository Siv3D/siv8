﻿//-----------------------------------------------
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

	constexpr RectF::RectF(const value_type _size) noexcept
		: pos{ 0, 0 }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Concept::Arithmetic auto _size) noexcept
		: pos{ 0, 0 }
		, size{ static_cast<value_type>(_size), static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const value_type _w, const value_type _h) noexcept
		: pos{ 0, 0 }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: pos{ 0, 0 }
		, size{ static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const size_type _size) noexcept
		: pos{ 0, 0 }
		, size{ _size } {}

	constexpr RectF::RectF(const value_type _x, const value_type _y, const value_type _size) noexcept
		: pos{ _x, _y }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _size) noexcept
		: pos{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, size{ static_cast<value_type>(_size), static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const value_type _x, const value_type _y, const value_type _w, const value_type _h) noexcept
		: pos{ _x, _y }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: pos{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, size{ static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const value_type _x, const value_type _y, const size_type _size) noexcept
		: pos{ _x, _y }
		, size{ _size } {}

	constexpr RectF::RectF(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y, const size_type _size) noexcept
		: pos{ static_cast<value_type>(_x), static_cast<value_type>(_y) }
		, size{ _size } {}

	constexpr RectF::RectF(const position_type _pos, const value_type _size) noexcept
		: pos{ _pos }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const position_type _pos, const Concept::Arithmetic auto _size) noexcept
		: pos{ _pos }
		, size{ static_cast<value_type>(_size), static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const position_type _pos, const value_type _w, const value_type _h) noexcept
		: pos{ _pos }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const position_type _pos, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: pos{ _pos }
		, size{ static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const position_type _pos, const size_type _size) noexcept
		: pos{ _pos }
		, size{ _size } {}

	constexpr RectF::RectF(const Rect& rect) noexcept
		: pos{ rect.pos }
		, size{ rect.size } {}

	constexpr RectF::RectF(const Arg::center_<position_type> _center, const value_type _size) noexcept
		: pos{ (_center->x - _size / 2), (_center->y - _size / 2) }
		, size{ _size, _size } {}
	
	constexpr RectF::RectF(const Arg::center_<position_type> _center, const Concept::Arithmetic auto _size) noexcept
		: RectF{ _center, static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const Arg::center_<position_type> _center, const value_type _w, const value_type _h) noexcept
		: pos{ (_center->x - _w / 2), (_center->y - _h / 2) }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Arg::center_<position_type> _center, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: RectF{ _center, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const Arg::center_<position_type> _center, const size_type _size) noexcept
		: pos{ (_center->x - _size.x / 2), (_center->y - _size.y / 2) }
		, size{ _size.x, _size.y } {}

	constexpr RectF::RectF(const Arg::topLeft_<position_type> topLeft, const value_type _size) noexcept
		: pos{ *topLeft }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Arg::topLeft_<position_type> topLeft, const Concept::Arithmetic auto _size) noexcept
		: RectF{ topLeft, static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const Arg::topLeft_<position_type> topLeft, const value_type _w, const value_type _h) noexcept
		: pos{ *topLeft }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Arg::topLeft_<position_type> topLeft, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: RectF{ topLeft, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const Arg::topLeft_<position_type> topLeft, const size_type _size) noexcept
		: pos{ *topLeft }
		, size{ _size.x, _size.y } {}

	constexpr RectF::RectF(const Arg::topCenter_<position_type> topCenter, const value_type _size) noexcept
		: pos{ (topCenter->x - _size / 2), topCenter->y }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Arg::topCenter_<position_type> topCenter, const Concept::Arithmetic auto _size) noexcept
		: RectF{ topCenter, static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const Arg::topCenter_<position_type> topCenter, const value_type _w, const value_type _h) noexcept
		: pos{ (topCenter->x - _w / 2), topCenter->y }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Arg::topCenter_<position_type> topCenter, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: RectF{ topCenter, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const Arg::topCenter_<position_type> topCenter, const size_type _size) noexcept
		: pos{ (topCenter->x - _size.x / 2), topCenter->y }
		, size{ _size.x, _size.y } {}

	constexpr RectF::RectF(const Arg::topRight_<position_type> topRight, const value_type _size) noexcept
		: pos{ (topRight->x - _size), topRight->y }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Arg::topRight_<position_type> topRight, const Concept::Arithmetic auto _size) noexcept
		: RectF{ topRight, static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const Arg::topRight_<position_type> topRight, const value_type _w, const value_type _h) noexcept
		: pos{ (topRight->x - _w), topRight->y }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Arg::topRight_<position_type> topRight, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: RectF{ topRight, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const Arg::topRight_<position_type> topRight, const size_type _size) noexcept
		: pos{ (topRight->x - _size.x), topRight->y }
		, size{ _size.x, _size.y } {}

	constexpr RectF::RectF(const Arg::rightCenter_<position_type> rightCenter, const value_type _size) noexcept
		: pos{ (rightCenter->x - _size), (rightCenter->y - _size / 2) }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Arg::rightCenter_<position_type> rightCenter, const Concept::Arithmetic auto _size) noexcept
		: RectF{ rightCenter, static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const Arg::rightCenter_<position_type> rightCenter, const value_type _w, const value_type _h) noexcept
		: pos{ (rightCenter->x - _w), (rightCenter->y - _h / 2) }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Arg::rightCenter_<position_type> rightCenter, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: RectF{ rightCenter, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const Arg::rightCenter_<position_type> rightCenter, const size_type _size) noexcept
		: pos{ (rightCenter->x - _size.x), (rightCenter->y - _size.y / 2) }
		, size{ _size.x, _size.y } {}

	constexpr RectF::RectF(const Arg::bottomRight_<position_type> bottomRight, const value_type _size) noexcept
		: pos{ (bottomRight->x - _size), (bottomRight->y - _size) }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Arg::bottomRight_<position_type> bottomRight, const Concept::Arithmetic auto _size) noexcept
		: RectF{ bottomRight, static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const Arg::bottomRight_<position_type> bottomRight, const value_type _w, const value_type _h) noexcept
		: pos{ (bottomRight->x - _w), (bottomRight->y - _h) }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Arg::bottomRight_<position_type> bottomRight, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: RectF{ bottomRight, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const Arg::bottomRight_<position_type> bottomRight, const size_type _size) noexcept
		: pos{ (bottomRight->x - _size.x), (bottomRight->y - _size.y) }
		, size{ _size.x, _size.y } {}

	constexpr RectF::RectF(const Arg::bottomCenter_<position_type> bottomCenter, const value_type _size) noexcept
		: pos{ (bottomCenter->x - _size / 2), (bottomCenter->y - _size) }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Arg::bottomCenter_<position_type> bottomCenter, const Concept::Arithmetic auto _size) noexcept
		: RectF{ bottomCenter, static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const Arg::bottomCenter_<position_type> bottomCenter, const value_type _w, const value_type _h) noexcept
		: pos{ (bottomCenter->x - _w / 2), (bottomCenter->y - _h) }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Arg::bottomCenter_<position_type> bottomCenter, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: RectF{ bottomCenter, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const Arg::bottomCenter_<position_type> bottomCenter, const size_type _size) noexcept
		: pos{ (bottomCenter->x - _size.x / 2), (bottomCenter->y - _size.y) }
		, size{ _size.x, _size.y } {}

	constexpr RectF::RectF(const Arg::bottomLeft_<position_type> bottomLeft, const value_type _size) noexcept
		: pos{ bottomLeft->x, (bottomLeft->y - _size) }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Arg::bottomLeft_<position_type> bottomLeft, const Concept::Arithmetic auto _size) noexcept
		: RectF{ bottomLeft, static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const Arg::bottomLeft_<position_type> bottomLeft, const value_type _w, const value_type _h) noexcept
		: pos{ bottomLeft->x, (bottomLeft->y - _h) }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Arg::bottomLeft_<position_type> bottomLeft, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: RectF{ bottomLeft, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const Arg::bottomLeft_<position_type> bottomLeft, const size_type _size) noexcept
		: pos{ bottomLeft->x, (bottomLeft->y - _size.y) }
		, size{ _size.x, _size.y } {}

	constexpr RectF::RectF(const Arg::leftCenter_<position_type> leftCenter, const value_type _size) noexcept
		: pos{ leftCenter->x, (leftCenter->y - _size / 2) }
		, size{ _size, _size } {}

	constexpr RectF::RectF(const Arg::leftCenter_<position_type> leftCenter, const Concept::Arithmetic auto _size) noexcept
		: RectF{ leftCenter, static_cast<value_type>(_size) } {}

	constexpr RectF::RectF(const Arg::leftCenter_<position_type> leftCenter, const value_type _w, const value_type _h) noexcept
		: pos{ leftCenter->x, (leftCenter->y - _h / 2) }
		, size{ _w, _h } {}

	constexpr RectF::RectF(const Arg::leftCenter_<position_type> leftCenter, const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) noexcept
		: RectF{ leftCenter, static_cast<value_type>(_w), static_cast<value_type>(_h) } {}

	constexpr RectF::RectF(const Arg::leftCenter_<position_type> leftCenter, const size_type _size) noexcept
		: pos{ leftCenter->x, (leftCenter->y - _size.y / 2) }
		, size{ _size.x, _size.y } {}

	////////////////////////////////////////////////////////////////
	//
	//	withX
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::withX(const value_type _x) const noexcept
	{
		return{ _x, pos.y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withY
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::withY(const value_type _y) const noexcept
	{
		return{ pos.x, _y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withPos
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::withPos(const value_type _x, const value_type _y) const noexcept
	{
		return{ _x, _y, size };
	}

	constexpr RectF RectF::withPos(const position_type _pos) const noexcept
	{
		return{ _pos, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withW
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::withW(const value_type _w) const noexcept
	{
		return{ pos, _w, size.y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withH
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::withH(const value_type _h) const noexcept
	{
		return{ pos, size.x, _h };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withSize
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::withSize(const value_type _w, const value_type _h) const noexcept
	{
		return{ pos, _w, _h };
	}

	constexpr RectF RectF::withSize(const size_type _size) const noexcept
	{
		return{ pos, _size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::withCenter(const value_type _cx, const value_type _cy) const noexcept
	{
		return{ (_cx - size.x / 2), (_cy - size.y / 2), size };
	}

	constexpr RectF RectF::withCenter(const position_type _center) const noexcept
	{
		return{ (_center.x - size.x / 2), (_center.y - size.y / 2), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF& RectF::setPos(const value_type _x, const value_type _y) noexcept
	{
		pos.set(_x, _y);
		return *this;
	}

	constexpr RectF& RectF::setPos(const position_type _pos) noexcept
	{
		pos = _pos;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF& RectF::setCenter(const value_type _x, const value_type _y) noexcept
	{
		pos.set((_x - size.x / 2), (_y - size.y / 2));
		return *this;
	}

	constexpr RectF& RectF::setCenter(const position_type _center) noexcept
	{
		pos.set((_center.x - size.x / 2), (_center.y - size.y / 2));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSize
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF& RectF::setSize(const value_type _size) noexcept
	{
		size.set(_size, _size);
		return *this;
	}

	constexpr RectF& RectF::setSize(const value_type _w, const value_type _h) noexcept
	{
		size.set(_w, _h);
		return *this;
	}

	constexpr RectF& RectF::setSize(const size_type _size) noexcept
	{
		size = _size;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF& RectF::set(const value_type _x, const value_type _y, const value_type _w, const value_type _h) noexcept
	{
		pos.set(_x, _y);
		size.set(_w, _h);
		return *this;
	}

	constexpr RectF& RectF::set(const value_type _x, const value_type _y, const value_type _size) noexcept
	{
		pos.set(_x, _y);
		size.set(_size, _size);
		return *this;
	}

	constexpr RectF& RectF::set(const value_type _x, const value_type _y, const size_type _size) noexcept
	{
		pos.set(_x, _y);
		size = _size;
		return *this;
	}

	constexpr RectF& RectF::set(const position_type _pos, const value_type _size) noexcept
	{
		pos = _pos;
		size.set(_size, _size);
		return *this;
	}

	constexpr RectF& RectF::set(const position_type _pos, const value_type _w, const value_type _h) noexcept
	{
		pos = _pos;
		size.set(_w, _h);
		return *this;
	}

	constexpr RectF& RectF::set(const position_type _pos, const size_type _size) noexcept
	{
		pos = _pos;
		size = _size;
		return *this;
	}

	constexpr RectF& RectF::set(const RectF& rect) noexcept
	{
		pos = rect.pos;
		size = rect.size;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::movedBy(const value_type _x, const value_type _y) const noexcept
	{
		return{ pos.movedBy(_x, _y), size };
	}

	constexpr RectF RectF::movedBy(const size_type v) const noexcept
	{
		return{ pos.movedBy(v), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF& RectF::moveBy(const value_type _x, const value_type _y) noexcept
	{
		pos.moveBy(_x, _y);
		return *this;
	}

	constexpr RectF& RectF::moveBy(const size_type v) noexcept
	{
		pos.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	stretched
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::stretched(const value_type xy) const noexcept
	{
		return stretched({ xy, xy });
	}

	constexpr RectF RectF::stretched(const value_type _x, const value_type _y) const noexcept
	{
		return stretched({ _x, _y });
	}

	constexpr RectF RectF::stretched(const size_type xy) const noexcept
	{
		return{ (pos - xy), (size + xy * 2) };
	}

	constexpr RectF RectF::stretched(const value_type top, const value_type right, const value_type bottom, const value_type left) const noexcept
	{
		return{ (pos.x - left), (pos.y - top), (size.x + left + right), (size.y + top + bottom) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::scaledFromCenter(const double s) const noexcept
	{
		return{ Arg::center = center(), (size * s) };
	}

	constexpr RectF RectF::scaledFromCenter(const double sx, const double sy) const noexcept
	{
		return{ Arg::center = center(), (size.x * sx), (size.y * sy) };
	}

	constexpr RectF RectF::scaledFromCenter(const Vec2 s) const noexcept
	{
		return{ Arg::center = center(), (size * s) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromTopLeft
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::scaledFromTopLeft(const double s) const noexcept
	{
		return{ pos, (size * s) };
	}

	constexpr RectF RectF::scaledFromTopLeft(const double sx, const double sy) const noexcept
	{
		return{ pos, (size.x * sx), (size.y * sy) };
	}

	constexpr RectF RectF::scaledFromTopLeft(const Vec2 s) const noexcept
	{
		return{ pos, (size * s) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::scaledFrom(const Vec2 _pos, const double s) const noexcept
	{
		return{ (_pos.x + (pos.x - _pos.x) * s), (_pos.y + (pos.y - _pos.y) * s), (size * s) };
	}

	constexpr RectF RectF::scaledFrom(const Vec2 _pos, const double sx, const double sy) const noexcept
	{
		return{ (_pos.x + (pos.x - _pos.x) * sx), (_pos.y + (pos.y - _pos.y) * sy), (size.x * sx), (size.y * sy) };
	}

	constexpr RectF RectF::scaledFrom(const Vec2 _pos, const Vec2 s) const noexcept
	{
		return{ (_pos.x + (pos.x - _pos.x) * s.x), (_pos.y + (pos.y - _pos.y) * s.y), (size * s) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF::operator bool() const noexcept
	{
		return hasArea();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool RectF::isEmpty() const noexcept
	{
		return ((size.x == 0) || (size.y == 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasArea
	//
	////////////////////////////////////////////////////////////////

	constexpr bool RectF::hasArea() const noexcept
	{
		return ((size.x != 0) && (size.y != 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	leftX, rightX, topY, bottomY
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF::value_type RectF::leftX() const noexcept
	{
		return pos.x;
	}

	constexpr RectF::value_type RectF::rightX() const noexcept
	{
		return (pos.x + size.x);
	}

	constexpr RectF::value_type RectF::topY() const noexcept
	{
		return pos.y;
	}

	constexpr RectF::value_type RectF::bottomY() const noexcept
	{
		return (pos.y + size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	centerX, centerY
	//
	////////////////////////////////////////////////////////////////

	constexpr double RectF::centerX() const noexcept
	{
		return (pos.x + size.x * 0.5);
	}

	constexpr double RectF::centerY() const noexcept
	{
		return (pos.y + size.y * 0.5);
	}

	////////////////////////////////////////////////////////////////
	//
	//	tl, tr, br, bl
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF::position_type RectF::tl() const noexcept
	{
		return pos;
	}

	constexpr RectF::position_type RectF::tr() const noexcept
	{
		return{ (pos.x + size.x), pos.y };
	}

	constexpr RectF::position_type RectF::br() const noexcept
	{
		return{ (pos.x + size.x), (pos.y + size.y) };
	}

	constexpr RectF::position_type RectF::bl() const noexcept
	{
		return{ pos.x, (pos.y + size.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	topCenter, rightCenter, bottomCenter, leftCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 RectF::topCenter() const noexcept
	{
		return{ (pos.x + size.x * 0.5), pos.y };
	}

	constexpr Vec2 RectF::rightCenter() const noexcept
	{
		return{ (pos.x + size.x), (pos.y + size.y * 0.5) };
	}

	constexpr Vec2 RectF::bottomCenter() const noexcept
	{
		return{ (pos.x + size.x * 0.5), (pos.y + size.y) };
	}

	constexpr Vec2 RectF::leftCenter() const noexcept
	{
		return{ pos.x, (pos.y + size.y * 0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	center
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 RectF::center() const noexcept
	{
		return{ (pos.x + size.x * 0.5), (pos.y + size.y * 0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getRelativePoint
	//
	////////////////////////////////////////////////////////////////

	constexpr Vec2 RectF::getRelativePoint(const double relativeX, const double relativeY) const noexcept
	{
		return{ (pos.x + size.x * relativeX), (pos.y + size.y * relativeY) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	top, right, bottom, left
	//
	////////////////////////////////////////////////////////////////

	constexpr Line RectF::top() const noexcept
	{
		return{ tl(), tr() };
	}

	constexpr Line RectF::right() const noexcept
	{
		return{ tr(), br() };
	}

	constexpr Line RectF::bottom() const noexcept
	{
		return{ br(), bl() };
	}

	constexpr Line RectF::left() const noexcept
	{
		return{ bl(), tl() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtIndex
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF::position_type RectF::pointAtIndex(const size_t index) const
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

	constexpr Line RectF::sideAtIndex(const size_t index) const
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

	constexpr Triangle RectF::triangleAtIndex(const size_t index) const
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

	constexpr RectF::value_type RectF::area() const noexcept
	{
		return (size.x * size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF::value_type RectF::perimeter() const noexcept
	{
		return ((size.x + size.y) * 2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	horizontalAspectRatio
	//
	////////////////////////////////////////////////////////////////

	constexpr double RectF::horizontalAspectRatio() const noexcept
	{
		if (size.y == 0)
		{
			return 0.0;
		}

		return (size.x / size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	copiedToTop
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::copiedToTop(const double distance) const noexcept
	{
		return{ pos.x, (pos.y - size.y - distance), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	copiedToRight
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::copiedToRight(const double distance) const noexcept
	{
		return{ (pos.x + size.x + distance), pos.y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	copiedToBottom
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::copiedToBottom(const double distance) const noexcept
	{
		return{ pos.x, (pos.y + size.y + distance), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	copiedToLeft
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::copiedToLeft(const double distance) const noexcept
	{
		return{ (pos.x - size.x - distance), pos.y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated90At
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::rotated90At(const position_type& _pos, const int32 n) const noexcept
	{
		switch (n % 4) // 時計回りに何回 90° 回転するか
		{
		case 1:
		case -3:
			return{ bl().rotate90At(_pos, 1),size.yx() }; // 1 回または -3 回
		case 2:
		case -2:
			return{ br().rotate90At(_pos, 2),size }; // 2 回または -2 回
		case 3:
		case -1:
			return{ tr().rotate90At(_pos, 3),size.yx() }; // 3 回または -1 回
		default:
			return *this; // 0 回
		}
	}

	constexpr RectF& RectF::rotate90At(const position_type& _pos, const int32 n) noexcept
	{
		return (*this = rotated90At(_pos, n));
	}

	////////////////////////////////////////////////////////////////
	//
	//	shearedX, shearedY
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad RectF::shearedX(const double vx) const noexcept
	{
		return{ { (pos.x + vx), pos.y },
				{ (pos.x + size.x + vx), pos.y },
				{ (pos.x + size.x - vx), (pos.y + size.y) },
				{ (pos.x - vx), (pos.y + size.y) } };
	}

	constexpr Quad RectF::shearedY(const double vy) const noexcept
	{
		return{ { pos.x, (pos.y - vy) },
				{ (pos.x + size.x), (pos.y + vy) },
				{ (pos.x + size.x), (pos.y + size.y + vy) },
				{ pos.x, (pos.y + size.y - vy) } };
	}

	////////////////////////////////////////////////////////////////
	//
	//	skewedX, skewedY
	//
	////////////////////////////////////////////////////////////////

	inline Quad RectF::skewedX(const double angle) const noexcept
	{
		return shearedX(std::tan(angle) * size.y / 2);
	}

	inline Quad RectF::skewedY(const double angle) const noexcept
	{
		return shearedY(std::tan(angle) * size.x / 2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rounded
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect RectF::rounded(const double r) const noexcept
	{
		return{ *this, Min(r, (size.x * 0.5), (size.y * 0.5)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	asQuad
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad RectF::asQuad() const noexcept
	{
		return{ tl(), tr(), br(), bl() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::lerp(const RectF& other, const double f) const noexcept
	{
		return{ pos.lerp(other.pos, f), size.lerp(other.size, f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOverlap
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::getOverlap(const RectF& other) const noexcept
	{
		const auto ox = Max(pos.x, other.pos.x);
		const auto oy = Max(pos.y, other.pos.y);
		const auto ow = (Min((pos.x + size.x), (other.pos.x + other.size.x)) - ox);

		if (0 <= ow)
		{
			const auto oh = (Min((pos.y + size.y), (other.pos.y + other.size.y)) - oy);

			if (0 <= oh)
			{
				return{ ox, oy, ow, oh };
			}
		}

		return Empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	clamped
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::clamped(const RectF& area) const noexcept
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

	inline uint64 RectF::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool RectF::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersect(*this, other);
	}






	////////////////////////////////////////////////////////////////
	//
	//	Empty
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::Empty() noexcept
	{
		return{ 0, 0, 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromPoints
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF RectF::FromPoints(const position_type a, const position_type b) noexcept
	{
		const auto [x0, x1] = MinMax(a.x, b.x);
		const auto [y0, y1] = MinMax(a.y, b.y);
		return{ x0, y0, (x1 - x0), (y1 - y0) };
	}
}

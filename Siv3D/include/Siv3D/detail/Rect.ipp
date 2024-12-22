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

	constexpr Rect Rect::withX(const Concept::Integral auto _x) const noexcept
	{
		return{ static_cast<value_type>(_x), pos.y, size };
	}

	constexpr RectF Rect::withX(const Concept::FloatingPoint auto _x) const noexcept
	{
		return{ static_cast<RectF::value_type>(_x), pos.y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withY
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withY(const Concept::Integral auto _y) const noexcept
	{
		return{ pos.x, static_cast<value_type>(_y), size };
	}

	constexpr RectF Rect::withY(const Concept::FloatingPoint auto _y) const noexcept
	{
		return{ pos.x, static_cast<RectF::value_type>(_y), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withPos
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withPos(const Concept::Integral auto _x, const Concept::Integral auto _y) const noexcept
	{
		return{ static_cast<value_type>(_x), static_cast<value_type>(_y), size };
	}

	constexpr RectF Rect::withPos(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y) const noexcept
	{
		return{ static_cast<RectF::value_type>(_x), static_cast<RectF::value_type>(_y), size };
	}

	constexpr Rect Rect::withPos(const position_type _pos) const noexcept
	{
		return{ _pos, size };
	}

	constexpr RectF Rect::withPos(const Vec2 _pos) const noexcept
	{
		return{ _pos, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withW
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withW(const Concept::Integral auto _w) const noexcept
	{
		return{ pos, static_cast<value_type>(_w), size.y };
	}

	constexpr RectF Rect::withW(const Concept::FloatingPoint auto _w) const noexcept
	{
		return{ pos, static_cast<RectF::value_type>(_w), size.y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withH
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withH(const Concept::Integral auto _h) const noexcept
	{
		return{ pos, size.x, static_cast<value_type>(_h) };
	}

	constexpr RectF Rect::withH(const Concept::FloatingPoint auto _h) const noexcept
	{
		return{ pos, size.x, static_cast<RectF::value_type>(_h) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withSize
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withSize(const Concept::Integral auto _w, const Concept::Integral auto _h) const noexcept
	{
		return{ pos, static_cast<value_type>(_w), static_cast<value_type>(_h) };
	}

	constexpr RectF Rect::withSize(const Concept::Arithmetic auto _w, const Concept::Arithmetic auto _h) const noexcept
	{
		return{ pos, static_cast<RectF::value_type>(_w), static_cast<RectF::value_type>(_h) };
	}

	constexpr Rect Rect::withSize(const size_type _size) const noexcept
	{
		return{ pos, _size };
	}

	constexpr RectF Rect::withSize(const SizeF _size) const noexcept
	{
		return{ pos, _size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::withCenter(const Concept::Integral auto _cx, const Concept::Integral auto _cy) const noexcept
	{
		return{ (static_cast<value_type>(_cx) - size.x / 2), (static_cast<value_type>(_cy) - size.y / 2), size };
	}

	constexpr RectF Rect::withCenter(const Concept::Arithmetic auto _cx, const Concept::Arithmetic auto _cy) const noexcept
	{
		return{ (static_cast<RectF::value_type>(_cx) - size.x / 2), (static_cast<RectF::value_type>(_cy) - size.y / 2), size };
	}

	constexpr Rect Rect::withCenter(const position_type _center) const noexcept
	{
		return{ (_center.x - size.x / 2), (_center.y - size.y / 2), size };
	}

	constexpr RectF Rect::withCenter(const Vec2 _center) const noexcept
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

	constexpr Rect& Rect::setCenter(const position_type _center) noexcept
	{
		pos.set((_center.x - size.x / 2), (_center.y - size.y / 2));
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

	constexpr Rect& Rect::set(const Rect& rect) noexcept
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

	constexpr Rect Rect::movedBy(const Concept::Integral auto _x, const Concept::Integral auto _y) const noexcept
	{
		return{ pos.movedBy(Point{ _x, _y }), size };
	}

	constexpr RectF Rect::movedBy(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y) const noexcept
	{
		return{ pos.movedBy(Vec2{ _x, _y }), size };
	}

	constexpr Rect Rect::movedBy(const size_type v) const noexcept
	{
		return{ pos.movedBy(v), size };
	}

	constexpr RectF Rect::movedBy(const Vec2 v) const noexcept
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

	constexpr Rect Rect::stretched(const Concept::Integral auto xy) const noexcept
	{
		return stretched(size_type{ xy, xy });
	}

	constexpr Rect Rect::stretched(const Concept::Integral auto _x, const Concept::Integral auto _y) const noexcept
	{
		return stretched(size_type{ _x, _y });
	}

	constexpr Rect Rect::stretched(const size_type xy) const noexcept
	{
		return{ (pos - xy), (size + xy * 2) };
	}

	constexpr Rect Rect::stretched(const Concept::Integral auto top, const Concept::Integral auto right, const Concept::Integral auto bottom, const Concept::Integral auto left) const noexcept
	{
		return{ (pos.x - left), (pos.y - top), (size.x + left + right), (size.y + top + bottom) };
	}

	constexpr RectF Rect::stretched(const Concept::FloatingPoint auto xy) const noexcept
	{
		return stretched(SizeF{ xy, xy });
	}

	constexpr RectF Rect::stretched(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y) const noexcept
	{
		return stretched(SizeF{ _x, _y });
	}

	constexpr RectF Rect::stretched(const SizeF xy) const noexcept
	{
		return{ (pos - xy), (size + xy * 2) };
	}

	constexpr RectF Rect::stretched(const Concept::Arithmetic auto top, const Concept::Arithmetic auto right, const Concept::Arithmetic auto bottom, const Concept::Arithmetic auto left) const noexcept
	{
		return{ (pos.x - left), (pos.y - top), (size.x + left + right), (size.y + top + bottom) };
	}

	template <Concept::Integral Integral>
	constexpr Rect Rect::stretched(const Arg::top_<Integral> top) const noexcept
	{
		return stretched(top.value(), 0, 0, 0);
	}

	template <Concept::Integral Integral>
	constexpr Rect Rect::stretched(const Arg::right_<Integral> right) const noexcept
	{
		return stretched(0, right.value(), 0, 0);
	}

	template <Concept::Integral Integral>
	constexpr Rect Rect::stretched(const Arg::bottom_<Integral> bottom) const noexcept
	{
		return stretched(0, 0, bottom.value(), 0);
	}

	template <Concept::Integral Integral>
	constexpr Rect Rect::stretched(const Arg::left_<Integral> left) const noexcept
	{
		return stretched(0, 0, 0, left.value());
	}

	template <Concept::FloatingPoint FloatingPoint>
	constexpr RectF Rect::stretched(const Arg::top_<FloatingPoint> top) const noexcept
	{
		return stretched(top.value(), 0, 0, 0);
	}

	template <Concept::FloatingPoint FloatingPoint>
	constexpr RectF Rect::stretched(const Arg::right_<FloatingPoint> right) const noexcept
	{
		return stretched(0, right.value(), 0, 0);
	}

	template <Concept::FloatingPoint FloatingPoint>
	constexpr RectF Rect::stretched(const Arg::bottom_<FloatingPoint> bottom) const noexcept
	{
		return stretched(0, 0, bottom.value(), 0);
	}

	template <Concept::FloatingPoint FloatingPoint>
	constexpr RectF Rect::stretched(const Arg::left_<FloatingPoint> left) const noexcept
	{
		return stretched(0, 0, 0, left.value());
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

	constexpr Rect::position_type Rect::tl() const noexcept
	{
		return pos;
	}

	constexpr Rect::position_type Rect::tr() const noexcept
	{
		return{ (pos.x + size.x), pos.y };
	}

	constexpr Rect::position_type Rect::br() const noexcept
	{
		return{ (pos.x + size.x), (pos.y + size.y) };
	}

	constexpr Rect::position_type Rect::bl() const noexcept
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
	//	copiedToTop
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::copiedToTop(const value_type distance) const noexcept
	{
		return{ pos.x, (pos.y - size.y - distance), size };
	}

	constexpr RectF Rect::copiedToTop(const Concept::FloatingPoint auto distance) const noexcept
	{
		return{ pos.x, (pos.y - size.y - distance), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	copiedToRight
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::copiedToRight(const value_type distance) const noexcept
	{
		return{ (pos.x + size.x + distance), pos.y, size };
	}

	constexpr RectF Rect::copiedToRight(const Concept::FloatingPoint auto distance) const noexcept
	{
		return{ (pos.x + size.x + distance), pos.y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	copiedToBottom
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::copiedToBottom(const value_type distance) const noexcept
	{
		return{ pos.x, (pos.y + size.y + distance), size };
	}

	constexpr RectF Rect::copiedToBottom(const Concept::FloatingPoint auto distance) const noexcept
	{
		return{ pos.x, (pos.y + size.y + distance), size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	copiedToLeft
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::copiedToLeft(const value_type distance) const noexcept
	{
		return{ (pos.x - size.x - distance), pos.y, size };
	}

	constexpr RectF Rect::copiedToLeft(const Concept::FloatingPoint auto distance) const noexcept
	{
		return{ (pos.x - size.x - distance), pos.y, size };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated90At
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::rotated90At(const position_type& _pos, const int32 n) const noexcept
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

	constexpr Rect& Rect::rotate90At(const position_type& _pos, const int32 n) noexcept
	{
		return (*this = rotated90At(_pos, n));
	}

	////////////////////////////////////////////////////////////////
	//
	//	shearedX, shearedY
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Rect::shearedX(const double vx) const noexcept
	{
		return{ { (pos.x + vx), pos.y },
				{ (pos.x + size.x + vx), pos.y },
				{ (pos.x + size.x - vx), (pos.y + size.y) },
				{ (pos.x - vx), (pos.y + size.y) } };
	}

	constexpr Quad Rect::shearedY(const double vy) const noexcept
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

	inline Quad Rect::skewedX(const double angle) const noexcept
	{
		return shearedX(std::tan(angle) * size.y / 2);
	}

	inline Quad Rect::skewedY(const double angle) const noexcept
	{
		return shearedY(std::tan(angle) * size.x / 2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rounded
	//
	////////////////////////////////////////////////////////////////

	constexpr RoundRect Rect::rounded(const double r) const noexcept
	{
		return{ *this, Min(r, (size.x * 0.5), (size.y * 0.5)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	asQuad
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Rect::asQuad() const noexcept
	{
		return{ tl(), tr(), br(), bl() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr RectF Rect::lerp(const Rect& other, const double f) const noexcept
	{
		return{ pos.lerp(other.pos, f), size.lerp(other.size, f) };
	}

	constexpr RectF Rect::lerp(const RectF& other, const double f) const noexcept
	{
		return{ pos.lerp(other.pos, f), size.lerp(other.size, f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOverlap
	//
	////////////////////////////////////////////////////////////////

	constexpr Rect Rect::getOverlap(const Rect& other) const noexcept
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

	constexpr RectF Rect::getOverlap(const RectF& other) const noexcept
	{
		return other.getOverlap(*this);
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

	constexpr RectF Rect::clamped(const RectF& area) const noexcept
	{
		const auto x0 = Clamp<RectF::value_type>(pos.x, area.pos.x, area.rightX());
		const auto y0 = Clamp<RectF::value_type>(pos.y, area.pos.y, area.bottomY());
		const auto x1 = Clamp<RectF::value_type>(rightX(), area.pos.x, area.rightX());
		const auto y1 = Clamp<RectF::value_type>(bottomY(), area.pos.y, area.bottomY());
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
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Shape2DType>
	constexpr bool Rect::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersect(*this, other);
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

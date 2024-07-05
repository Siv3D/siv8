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

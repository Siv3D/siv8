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

	constexpr Line::Line(const value_type startX, const value_type startY, const value_type endX, const value_type endY) noexcept
		: start{ startX, startY }
		, end{ endX, endY } {}

	constexpr Line::Line(const Concept::Arithmetic auto startX, const Concept::Arithmetic auto startY, const Concept::Arithmetic auto endX, const Concept::Arithmetic auto endY) noexcept
		: start{ static_cast<value_type>(startX), static_cast<value_type>(startY) }
		, end{ static_cast<value_type>(endX), static_cast<value_type>(endY) } {}

	constexpr Line::Line(const position_type _start, const value_type endX, const value_type endY) noexcept
		: start{ _start }
		, end{ endX, endY } {}

	constexpr Line::Line(const position_type _start, const Concept::Arithmetic auto endX, const Concept::Arithmetic auto endY) noexcept
		: start{ _start }
		, end{ static_cast<value_type>(endX), static_cast<value_type>(endY) } {}

	constexpr Line::Line(const value_type startX, const value_type startY, const position_type _end) noexcept
		: start{ startX, startY }
		, end{ _end } {}

	constexpr Line::Line(const Concept::Arithmetic auto startX, const Concept::Arithmetic auto startY, const position_type _end) noexcept
		: start{ static_cast<value_type>(startX), static_cast<value_type>(startY) }
		, end{ _end } {}

	constexpr Line::Line(const position_type _start, const position_type _end) noexcept
		: start{ _start }
		, end{ _end } {}

	constexpr Line::Line(const value_type originX, const value_type originY, const Arg::direction_<vector_type> vector) noexcept
		: start{ originX, originY }
		, end{ start + *vector } {}

	constexpr Line::Line(const Concept::Arithmetic auto originX, const Concept::Arithmetic auto originY, Arg::direction_<vector_type> vector) noexcept
		: start{ static_cast<value_type>(originX), static_cast<value_type>(originY) }
		, end{ start + *vector } {}

	constexpr Line::Line(const position_type origin, const Arg::direction_<vector_type> direction) noexcept
		: start{ origin }
		, end{ start + *direction } {}

	inline Line::Line(const value_type originX, const value_type originY, const Arg::angle_<value_type> angle, const value_type length) noexcept
		: start{ originX, originY }
		, end{ start + Circular{ length, *angle } } {}

	Line::Line(const value_type originX, const value_type originY, const Arg::angle_<value_type> angle, const Concept::Arithmetic auto length) noexcept
		: start{ originX, originY }
		, end{ start + Circular{ static_cast<value_type>(length), *angle } } {}

	inline Line::Line(const position_type origin, const Arg::angle_<value_type> angle, const value_type length) noexcept
		: start{ origin }
		, end{ start + Circular{ length, *angle } } {}

	Line::Line(const position_type origin, const Arg::angle_<value_type> angle, const Concept::Arithmetic auto length) noexcept
		: start{ origin }
		, end{ start + Circular{ static_cast<value_type>(length), *angle } } {}

	////////////////////////////////////////////////////////////////
	//
	//	withStart
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Line::withStart(const value_type startX, const value_type startY) const noexcept
	{
		return{ startX, startY, end };
	}

	constexpr Line Line::withStart(const position_type _start) const noexcept
	{
		return{ _start, end };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withEnd
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Line::withEnd(const value_type endX, const value_type endY) const noexcept
	{
		return{ start, endX, endY };
	}

	constexpr Line Line::withEnd(const position_type _end) const noexcept
	{
		return{ start, _end };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setStart
	//
	////////////////////////////////////////////////////////////////

	constexpr Line& Line::setStart(const value_type startX, const value_type startY) noexcept
	{
		start.set(startX, startY);
		return *this;
	}

	constexpr Line& Line::setStart(const position_type _start) noexcept
	{
		start.set(_start);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setEnd
	//
	////////////////////////////////////////////////////////////////

	constexpr Line& Line::setEnd(const value_type endX, const value_type endY) noexcept
	{
		end.set(endX, endY);
		return *this;
	}

	constexpr Line& Line::setEnd(const position_type _end) noexcept
	{
		end.set(_end);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr Line& Line::set(const value_type x0, const value_type y0, const value_type x1, const value_type y1) noexcept
	{
		start.set(x0, y0);
		end.set(x1, y1);
		return *this;
	}

	constexpr Line& Line::set(const position_type p0, const value_type x1, const value_type y1) noexcept
	{
		start.set(p0);
		end.set(x1, y1);
		return *this;
	}

	constexpr Line& Line::set(const value_type x0, const value_type y0, const position_type p1) noexcept
	{
		start.set(x0, y0);
		end.set(p1);
		return *this;
	}

	constexpr Line& Line::set(const position_type p0, const position_type p1) noexcept
	{
		start.set(p0);
		end.set(p1);
		return *this;
	}

	constexpr Line& Line::set(const Line& line) noexcept
	{
		start.set(line.start);
		end.set(line.end);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Line::movedBy(const value_type x, const value_type y) const noexcept
	{
		return{ start.movedBy(x, y), end.movedBy(x, y) };
	}

	constexpr Line Line::movedBy(const vector_type v) const noexcept
	{
		return{ start.movedBy(v), end.movedBy(v) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Line& Line::moveBy(const value_type x, const value_type y) noexcept
	{
		start.moveBy(x, y);
		end.moveBy(x, y);
		return *this;
	}

	constexpr Line& Line::moveBy(const vector_type v) noexcept
	{
		start.moveBy(v);
		end.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	stretched
	//
	////////////////////////////////////////////////////////////////

	inline Line Line::stretched(const value_type extentionBothSides) const noexcept
	{
		const vector_type v = vector().withLength(extentionBothSides);
		return{ (start - v), (end + v) };
	}

	inline Line Line::stretched(const value_type extentionStart, const value_type extentionEnd) const noexcept
	{
		const vector_type v = normalizedVector();
		return{ (start - v * extentionStart), (end + v * extentionEnd) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromStart
	//
	////////////////////////////////////////////////////////////////

	inline Line Line::scaledFromStart(const value_type scale) const noexcept
	{
		return{ start, (start + (end - start) * scale) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromEnd
	//
	////////////////////////////////////////////////////////////////

	inline Line Line::scaledFromEnd(const value_type scale) const noexcept
	{
		return{ (end + (start - end) * scale), end };
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaledFromCenter
	//
	////////////////////////////////////////////////////////////////

	inline Line Line::scaledFromCenter(const value_type scale) const noexcept
	{
		const position_type c = center();
		const vector_type v = (vector() * scale / 2);
		return{ (c - v), (c + v) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAtStart
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	inline Line Line::rotatedAtStart(const double angle) const noexcept
	{
		return{ start, start.movedBy(vector().rotated(angle)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotateAtStart
	//
	////////////////////////////////////////////////////////////////

	inline Line& Line::rotateAtStart(const double angle) noexcept
	{
		end = start.movedBy(vector().rotated(angle));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAtEnd
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	inline Line Line::rotatedAtEnd(const double angle) const noexcept
	{
		return{ end.movedBy((-vector()).rotated(angle)), end };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotateAtEnd
	//
	////////////////////////////////////////////////////////////////

	inline Line& Line::rotateAtEnd(const double angle) noexcept
	{
		start = end.movedBy((-vector()).rotated(angle));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAtCenter
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	inline Line Line::rotatedAtCenter(const double angle) const noexcept
	{
		const position_type c = center();
		const Vec2 hv = (vector().rotated(angle) * 0.5);
		return{ (c - hv), (c + hv) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotateAtCenter
	//
	////////////////////////////////////////////////////////////////

	inline Line& Line::rotateAtCenter(const double angle) noexcept
	{
		const position_type c = center();
		const Vec2 hv = (vector().rotated(angle) * 0.5);
		start = (c - hv);
		end = (c + hv);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	vector
	//
	////////////////////////////////////////////////////////////////

	constexpr Line::vector_type Line::vector() const noexcept
	{
		return (end - start);
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalizedVector
	//
	////////////////////////////////////////////////////////////////

	inline Line::vector_type Line::normalizedVector() const noexcept
	{
		return vector().normalized();
	}

	////////////////////////////////////////////////////////////////
	//
	//	perpendicularUnitVector
	//
	////////////////////////////////////////////////////////////////

	inline Line::vector_type Line::perpendicularUnitVector() const noexcept
	{
		return Vec2{ (end.y - start.y), (start.x - end.x) }.normalized();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reversed
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Line::reversed() const noexcept
	{
		return{ end, start };
	}

	////////////////////////////////////////////////////////////////
	//
	//	reverse
	//
	////////////////////////////////////////////////////////////////

	constexpr Line& Line::reverse() noexcept
	{
		std::ranges::swap(start, end);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasLength
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Line::hasLength() const noexcept
	{
		return (start != end);
	}

	////////////////////////////////////////////////////////////////
	//
	//	length
	//
	////////////////////////////////////////////////////////////////

	inline Line::value_type Line::length() const noexcept
	{
		return start.distanceFrom(end);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lengthSq
	//
	////////////////////////////////////////////////////////////////

	constexpr Line::value_type Line::lengthSq() const noexcept
	{
		return start.distanceFromSq(end);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtIndex
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	inline Line::position_type& Line::pointAtIndex(const size_t index) noexcept
	{
		if (index == 0)
		{
			return start;
		}
		else if (index == 1)
		{
			return end;
		}
		else
		{
			ThrowPointAtIndexOutOfRange();
		}
	}

	[[nodiscard]]
	inline const Line::position_type& Line::pointAtIndex(const size_t index) const noexcept
	{
		if (index == 0)
		{
			return start;
		}
		else if (index == 1)
		{
			return end;
		}
		else
		{
			ThrowPointAtIndexOutOfRange();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	interpolatedPointAt
	//
	////////////////////////////////////////////////////////////////

	constexpr Line::position_type Line::interpolatedPointAt(const double t) const noexcept
	{
		return start.lerp(end, t);
	}

	////////////////////////////////////////////////////////////////
	//
	//	center
	//
	////////////////////////////////////////////////////////////////

	constexpr Line::position_type Line::center() const noexcept
	{
		return{ ((start.x + end.x) * 0.5), ((start.y + end.y) * 0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	withCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Line::withCenter(const value_type x, const value_type y) const noexcept
	{
		return withCenter({ x, y });
	}

	constexpr Line Line::withCenter(const position_type pos) const noexcept
	{
		const position_type c = center();
		const vector_type v = (pos - c);
		return{ (start + v), (end + v) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	setCenter
	//
	////////////////////////////////////////////////////////////////

	constexpr Line& Line::setCenter(const value_type x, const value_type y) noexcept
	{
		return setCenter({ x, y });
	}

	constexpr Line& Line::setCenter(const position_type pos) noexcept
	{
		const position_type c = center();
		const vector_type v = (pos - c);
		start.moveBy(v);
		end.moveBy(v);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	constexpr Line Line::lerp(const Line& other, const double f) const noexcept
	{
		return{ start.lerp(other.start, f), end.lerp(other.end, f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Line::hash() const noexcept
	{
		return Hash(*this);
	}








}

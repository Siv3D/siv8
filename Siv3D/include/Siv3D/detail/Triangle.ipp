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

	constexpr Triangle::Triangle(const value_type sides) noexcept
		: p0{ 0.0, (-Math::InvSqrt3 * sides) }
		, p1{ (sides * 0.5), (sides / (2.0 * Math::Sqrt3)) }
		, p2{ -p1.x, p1.y } {}

	constexpr Triangle::Triangle(const Concept::Arithmetic auto sides) noexcept
		: Triangle{ static_cast<value_type>(sides) } {}

	Triangle::Triangle(const Concept::Arithmetic auto sides, const Concept::Arithmetic auto angle) noexcept
		: Triangle{ static_cast<value_type>(sides), static_cast<value_type>(angle) } {}

	constexpr Triangle::Triangle(const value_type x, const value_type y, const value_type sides) noexcept
		: Triangle{ Triangle{ sides }.moveBy(x, y) } {}

	constexpr Triangle::Triangle(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y, const Concept::Arithmetic auto sides) noexcept
		: Triangle{ static_cast<value_type>(x), static_cast<value_type>(y), static_cast<value_type>(sides) } {}

	constexpr Triangle::Triangle(const position_type& pos, const value_type sides) noexcept
		: Triangle{ pos.x, pos.y, sides } {}

	constexpr Triangle::Triangle(const position_type& pos, const Concept::Arithmetic auto sides) noexcept
		: Triangle{ pos.x, pos.y, static_cast<value_type>(sides) } {}

	inline Triangle::Triangle(const value_type x, const value_type y, const value_type sides, const value_type angle) noexcept
		: Triangle{ Triangle{ sides, angle }.moveBy(x, y) } {}

	Triangle::Triangle(const Concept::Arithmetic auto x, const Concept::Arithmetic auto y, const Concept::Arithmetic auto sides, const value_type angle) noexcept
		: Triangle{ Triangle{ static_cast<value_type>(sides), static_cast<value_type>(angle) }.moveBy(static_cast<value_type>(x), static_cast<value_type>(y)) } {}

	inline Triangle::Triangle(const position_type& pos, const value_type sides, const value_type angle) noexcept
		: Triangle{ pos.x, pos.y, sides, angle } {}

	Triangle::Triangle(const position_type& pos, const Concept::Arithmetic auto sides, const value_type angle) noexcept
		: Triangle{ pos.x, pos.y, static_cast<value_type>(sides), angle } {}

	constexpr Triangle::Triangle(const value_type x0, const value_type y0, const value_type x1, const value_type y1, const value_type x2, const value_type y2) noexcept
		: p0{ x0, y0 }
		, p1{ x1, y1 }
		, p2{ x2, y2 } {}

	constexpr Triangle::Triangle(const Concept::Arithmetic auto x0, const Concept::Arithmetic auto y0, const Concept::Arithmetic auto x1, const Concept::Arithmetic auto y1, const Concept::Arithmetic auto x2, const Concept::Arithmetic auto y2) noexcept
		: p0{ static_cast<value_type>(x0), static_cast<value_type>(y0) }
		, p1{ static_cast<value_type>(x1), static_cast<value_type>(y1) }
		, p2{ static_cast<value_type>(x2), static_cast<value_type>(y2) } {}

	constexpr Triangle::Triangle(const position_type& _p0, const position_type& _p1, const position_type& _p2) noexcept
		: p0{ _p0 }
		, p1{ _p1 }
		, p2{ _p2 } {}




	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle& Triangle::moveBy(const value_type x, const value_type y) noexcept
	{
		p0.moveBy(x, y);
		p1.moveBy(x, y);
		p2.moveBy(x, y);
		return *this;
	}

	constexpr Triangle& Triangle::moveBy(const position_type v) noexcept
	{
		p0.moveBy(v);
		p1.moveBy(v);
		p2.moveBy(v);
		return *this;
	}



	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	constexpr Triangle::value_type Triangle::area() const noexcept
	{
		return (Abs((p0.x - p2.x) * (p1.y - p0.y) - (p0.x - p1.x) * (p2.y - p0.y)) * 0.5);
	}

}

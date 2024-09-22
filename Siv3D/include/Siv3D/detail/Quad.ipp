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

	constexpr Quad::Quad(const value_type x0, const value_type y0, const value_type x1, const value_type y1, const value_type x2, const value_type y2, const value_type x3, const value_type y3) noexcept
		: p0{ x0, y0 }
		, p1{ x1, y1 }
		, p2{ x2, y2 }
		, p3{ x3, y3 } {}

	constexpr Quad::Quad(const Concept::Arithmetic auto x0, const Concept::Arithmetic auto y0, const Concept::Arithmetic auto x1, const Concept::Arithmetic auto y1, const Concept::Arithmetic auto x2, const Concept::Arithmetic auto y2, const Concept::Arithmetic auto x3, const Concept::Arithmetic auto y3) noexcept
		: p0{ static_cast<value_type>(x0), static_cast<value_type>(y0) }
		, p1{ static_cast<value_type>(x1), static_cast<value_type>(y1) }
		, p2{ static_cast<value_type>(x2), static_cast<value_type>(y2) }
		, p3{ static_cast<value_type>(x3), static_cast<value_type>(y3) } {}

	constexpr Quad::Quad(const position_type& _p0, const position_type& _p1, const position_type& _p2, const position_type& _p3) noexcept
		: p0{ _p0 }
		, p1{ _p1 }
		, p2{ _p2 }
		, p3{ _p3 } {}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad Quad::movedBy(const value_type x, const value_type y) const noexcept
	{
		return{ p0.movedBy(x, y), p1.movedBy(x, y), p2.movedBy(x, y), p3.movedBy(x, y) };
	}

	constexpr Quad Quad::movedBy(const position_type v) const noexcept
	{
		return movedBy(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Quad& Quad::moveBy(const value_type x, const value_type y) noexcept
	{
		p0.moveBy(x, y);
		p1.moveBy(x, y);
		p2.moveBy(x, y);
		p3.moveBy(x, y);
		return *this;
	}

	constexpr Quad& Quad::moveBy(const position_type v) noexcept
	{
		return moveBy(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pointAtIndex
	//
	////////////////////////////////////////////////////////////////

	inline Quad::position_type& Quad::pointAtIndex(const size_t index)
	{
		if (index == 0)
		{
			return p0;
		}
		else if (index == 1)
		{
			return p1;
		}
		else if (index == 2)
		{
			return p2;
		}
		else if (index == 3)
		{
			return p3;
		}
		else
		{
			ThrowPointAtIndexOutOfRange();
		}
	}

	inline const Quad::position_type& Quad::pointAtIndex(const size_t index) const
	{
		if (index == 0)
		{
			return p0;
		}
		else if (index == 1)
		{
			return p1;
		}
		else if (index == 2)
		{
			return p2;
		}
		else if (index == 3)
		{
			return p3;
		}
		else
		{
			ThrowPointAtIndexOutOfRange();
		}
	}

	constexpr Triangle Quad::triangleAtIndex(const size_t index) const
	{
		if (index == 0)
		{
			return{ p0, p1, p3 };
		}
		else if (index == 1)
		{
			return{ p3, p1, p2 };
		}
		else
		{
			ThrowTriangleAtIndexOutOfRange();
		}
	}
}

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

	constexpr Point::Point(const value_type _x, const value_type _y) noexcept
		: x{ _x }
		, y{ _y } {}

	constexpr Point::Point(const Concept::Integral auto _x, const Concept::Integral auto _y) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ static_cast<value_type>(_y) } {}

	////////////////////////////////////////////////////////////////
	//
	//	elem
	//
	////////////////////////////////////////////////////////////////

	constexpr Point::value_type Point::elem(const size_t index) const noexcept
	{
		if (index == 0)
		{
			return x;
		}
		else if (index == 1)
		{
			return y;
		}
		else
		{
			return 0;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPointer
	//
	////////////////////////////////////////////////////////////////

	constexpr Point::value_type* Point::getPointer() noexcept
	{
		return &x;
	}

	constexpr const Point::value_type* Point::getPointer() const noexcept
	{
		return &x;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::operator +() const noexcept
	{
		return *this;
	}

	constexpr Point Point::operator +(const Point p) const noexcept
	{
		return{ (x + p.x), (y + p.y) };
	}

	template <class Type>
	constexpr Vector2D<Type> Point::operator +(const Vector2D<Type> v) const noexcept
	{
		return{ (x + v.x), (y + v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::operator -() const noexcept
	{
		return{ -x, -y };
	}

	constexpr Point Point::operator -(const Point p) const noexcept
	{
		return{ (x - p.x), (y - p.y) };
	}

	template <class Type>
	constexpr Vector2D<Type> Point::operator -(const Vector2D<Type> v) const noexcept
	{
		return{ (x - v.x), (y - v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::operator *(const int32 s) const noexcept
	{
		return{ (x * s), (y * s) };
	}

	constexpr Float2 Point::operator * (const float s) const noexcept
	{
		return{ (x * s), (y * s) };
	}

	constexpr Vec2 Point::operator * (const double s) const noexcept
	{
		return{ (x * s), (y * s) };
	}

	constexpr Point Point::operator *(const Point p) const noexcept
	{
		return{ (x * p.x), (y * p.y) };
	}

	template <class Type>
	constexpr Vector2D<Type> Point::operator *(const Vector2D<Type> v) const noexcept
	{
		return{ (x * v.x), (y * v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::operator /(const int32 s) const noexcept
	{
		return{ (x / s), (y / s) };
	}

	constexpr Float2 Point::operator /(const float s) const noexcept
	{
		return{ (x / s), (y / s) };
	}

	constexpr Vec2 Point::operator /(const double s) const noexcept
	{
		return{ (x / s), (y / s) };
	}

	constexpr Point Point::operator /(const Point p) const noexcept
	{
		return{ (x / p.x), (y / p.y) };
	}

	template <class Type>
	constexpr Vector2D<Type> Point::operator /(const Vector2D<Type> v) const noexcept
	{
		return{ (x / v.x), (y / v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator %
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::operator %(const int32 s) const noexcept
	{
		return{ (x % s), (y % s) };
	}

	constexpr Point Point::operator %(const Point p) const noexcept
	{
		return{ (x % p.x), (y % p.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	constexpr Point& Point::operator +=(const Point p) noexcept
	{
		x += p.x;
		y += p.y;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	constexpr Point& Point::operator -=(const Point p) noexcept
	{
		x -= p.x;
		y -= p.y;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	constexpr Point& Point::operator *=(const int32 s) noexcept
	{
		x *= s;
		y *= s;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	constexpr Point& Point::operator /=(const int32 s) noexcept
	{
		assert(s != 0);
		x /= s;
		y /= s;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator %=
	//
	////////////////////////////////////////////////////////////////

	constexpr Point& Point::operator %=(const int32 s) noexcept
	{
		assert(s != 0);
		x %= s;
		y %= s;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isZero
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Point::isZero() const noexcept
	{
		return ((x == 0) && (y == 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	minComponent, maxComponent
	//
	////////////////////////////////////////////////////////////////

	constexpr Point::value_type Point::minComponent() const noexcept
	{
		return Min(x, y);
	}

	constexpr Point::value_type Point::maxComponent() const noexcept
	{
		return Max(x, y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	constexpr void Point::clear() noexcept
	{
		x = 0;
		y = 0;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withX, withY
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::withX(const value_type _x) const noexcept
	{
		return{ _x, y };
	}

	constexpr Point Point::withY(const value_type _y) const noexcept
	{
		return{ x, _y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	constexpr Point& Point::set(const value_type _x, const value_type _y) noexcept
	{
		x = _x;
		y = _y;
		return *this;
	}

	constexpr Point& Point::set(const Point p) noexcept
	{
		x = p.x;
		y = p.y;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::movedBy(const value_type _x, const value_type _y) const noexcept
	{
		return{ (x + _x), (y + _y) };
	}

	constexpr Point Point::movedBy(const Point p) const noexcept
	{
		return{ (x + p.x), (y + p.y) };
	}

	template <class Type>
	constexpr Vector2D<Type> Point::movedBy(const Vector2D<Type> v) const noexcept
	{
		return{ (x + v.x), (y + v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	constexpr Point& Point::moveBy(const value_type _x, const value_type _y) noexcept
	{
		x += _x;
		y += _y;
		return *this;
	}

	constexpr Point& Point::moveBy(const Point p) noexcept
	{
		x += p.x;
		y += p.y;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	horizontalAspectRatio
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Type Point::horizontalAspectRatio() const noexcept
	{
		if (y == 0)
		{
			return 0;
		}

		return (static_cast<Type>(x) / y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	length
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	inline Type Point::length() const noexcept
	{
		const Type x_ = static_cast<Type>(x);
		const Type y_ = static_cast<Type>(y);
		return static_cast<Type>(std::sqrt((x_ * x_) + (y_ * y_)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	lengthSq
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Type Point::lengthSq() const noexcept
	{
		const Type x_ = static_cast<Type>(x);
		const Type y_ = static_cast<Type>(y);
		return ((x_ * x_) + (y_ * y_));
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanLength
	//
	////////////////////////////////////////////////////////////////

	constexpr int32 Point::manhattanLength() const noexcept
	{
		return (Abs(x) + Abs(y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr int32 Point::manhattanDistanceFrom(const int32 _x, const int32 _y) const noexcept
	{
		const auto xMinMax = std::minmax(x, _x);
		const auto yMinMax = std::minmax(y, _y);
		return ((xMinMax.second - xMinMax.first) + (yMinMax.second - yMinMax.first));
	}

	constexpr int32 Point::manhattanDistanceFrom(const Point p) const noexcept
	{
		return manhattanDistanceFrom(p.x, p.y);
	}

	template <class Type>
	constexpr Type Point::manhattanDistanceFrom(const Vector2D<Type> p) const noexcept
	{
		const Type x_ = static_cast<Type>(x);
		const Type y_ = static_cast<Type>(y);
		const auto xMinMax = std::minmax(x_, p.x);
		const auto yMinMax = std::minmax(y_, p.y);
		return ((xMinMax.second - xMinMax.first) + (yMinMax.second - yMinMax.first));
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFrom
	//
	////////////////////////////////////////////////////////////////

	inline double Point::distanceFrom(const double _x, const double _y) const noexcept
	{
		return std::sqrt(distanceFromSq(_x, _y));
	}

	inline double Point::distanceFrom(const Point p) const noexcept
	{
		return distanceFrom(p.x, p.y);
	}

	template <class Type>
	Type Point::distanceFrom(const Vector2D<Type> p) const noexcept
	{
		const Type xx = (x - p.x);
		const Type yy = (y - p.y);
		return std::sqrt((xx * xx) + (yy * yy));
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFrom
	//
	////////////////////////////////////////////////////////////////

	constexpr double Point::distanceFromSq(const double _x, const double _y) const noexcept
	{
		const double xx = (x - _x);
		const double yy = (y - _y);
		return ((xx * xx) + (yy * yy));
	}

	constexpr double Point::distanceFromSq(const Point p) const noexcept
	{
		return distanceFromSq(p.x, p.y);
	}

	template <class Type>
	constexpr Type Point::distanceFromSq(const Vector2D<Type> p) const noexcept
	{
		const Type xx = (x - p.x);
		const Type yy = (y - p.y);
		return ((xx * xx) + (yy * yy));
	}

	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	constexpr int32 Point::area() const noexcept
	{
		return (x * y);
	}

}

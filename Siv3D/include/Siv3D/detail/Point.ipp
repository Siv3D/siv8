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

	template <class Type>
	constexpr Type Point::area() const noexcept
	{
		return (static_cast<Type>(x) * y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate90, rotated90
	//
	////////////////////////////////////////////////////////////////

	constexpr Point& Point::rotate90(const int32 n) noexcept
	{
		return (*this = rotated90(n));
	}

	constexpr Point Point::rotated90(const int32 n) const noexcept
	{
		switch (n % 4) // 時計回りに何回 90° 回転するか
		{
		case 1:
		case -3:
			return{ -y, x }; // 1 回または -3 回
		case 2:
		case -2:
			return{ -x, -y }; // 2 回または -2 回
		case 3:
		case -1:
			return{ y, -x }; // 3 回または -1 回
		default:
			return *this; // 0 回
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate90, rotated90
	//
	////////////////////////////////////////////////////////////////

	constexpr Point& Point::rotate90At(const Point center, const int32 n) noexcept
	{
		return (*this = rotated90At(center, n));
	}

	constexpr Point Point::rotated90At(const Point center, const int32 n) const noexcept
	{
		return ((*this - center).rotated90(n) + center);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAngle
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Type Point::getAngle() const noexcept
	{
		if (isZero())
		{
			return Math::NaN_v<Type>;
		}

		return std::atan2(x, -y);
	}

	template <class Type>
	Type Point::getAngle(const Vector2D<Type> other) const noexcept
	{
		if (isZero() || other.isZero())
		{
			return Math::NaN_v<Type>;
		}

		return std::atan2(Vector2D<Type>{ *this }.cross(other), Vector2D<Type>{ *this }.dot(other));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPerpendicularCW, getPerpendicularCCW
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::getPerpendicularCW() const noexcept
	{
		return{ -y, x };
	}

	constexpr Point Point::getPerpendicularCCW() const noexcept
	{
		return{ y, -x };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMidpoint
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Point::getMidpoint(const Point other) const noexcept
	{
		return{ (x + (other.x - x) * 0.5), (y + (other.y - y) * 0.5) };
	}

	template <class Type>
	constexpr Vector2D<Type> Point::getMidpoint(const Vector2D<Type> other) const noexcept
	{
		return{ (x * 0.5 + other.x * 0.5), (y * 0.5 + other.y * 0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPointByAngleAndDistance
	//
	////////////////////////////////////////////////////////////////

	auto Point::getPointByAngleAndDistance(const Concept::Arithmetic auto angle, const Concept::Arithmetic auto distance) const noexcept
	{
		using ValueType = CommonFloat_t<decltype(angle), decltype(distance)>;
		
		const auto s = std::sin(angle);
		const auto c = std::cos(angle);
		
		return Vector2D<ValueType>{
			static_cast<ValueType>((s * distance) + x),
			static_cast<ValueType>((-c * distance) + y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Point::lerp(const Point other, const double f) const noexcept
	{
		return{ (x + (other.x - x) * f), (y + (other.y - y) * f) };
	}

	template <class Type>
	constexpr Vector2D<Type> Point::lerp(const Vector2D<Type> other, const double f) const noexcept
	{
		return{ (x + (other.x - x) * static_cast<Type>(f)), (y + (other.y - y) * static_cast<Type>(f)) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Point::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	xx, xy, yx, yy, x0, y0
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::xx() const noexcept
	{
		return{ x, x };
	}

	constexpr Point Point::xy() const noexcept
	{
		return{ x, y };
	}

	constexpr Point Point::yx() const noexcept
	{
		return{ y, x };
	}

	constexpr Point Point::yy() const noexcept
	{
		return{ y, y };
	}

	constexpr Point Point::x0() const noexcept
	{
		return{ x, 0 };
	}

	constexpr Point Point::y0() const noexcept
	{
		return{ y, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::Zero() noexcept
	{
		return{ 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	One
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::One() noexcept
	{
		return{ 1, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	All
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::All(const value_type value) noexcept
	{
		return{ value, value };
	}

	////////////////////////////////////////////////////////////////
	//
	//	UnitX, UnitY
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::UnitX() noexcept
	{
		return{ 1, 0 };
	}

	constexpr Point Point::UnitY() noexcept
	{
		return{ 0, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Left, Right, Up, Down
	//
	////////////////////////////////////////////////////////////////

	constexpr Point Point::Left(const value_type length) noexcept
	{
		return{ -length, 0 };
	}

	constexpr Point Point::Right(const value_type length) noexcept
	{
		return{ length, 0 };
	}

	constexpr Point Point::Up(const value_type length) noexcept
	{
		return{ 0, -length };
	}

	constexpr Point Point::Down(const value_type length) noexcept
	{
		return{ 0, length };
	}
}

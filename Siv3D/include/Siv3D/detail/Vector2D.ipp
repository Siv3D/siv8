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
	namespace Geometry2D
	{
		[[nodiscard]]
		constexpr bool Intersect(const Vec2& a, const Point& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersect(const Vec2& a, const Vec2& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersect(const Vec2& a, const Line& b) noexcept;

		[[nodiscard]]
		bool Intersect(const Vec2& a, const Bezier2& b);

		//[[nodiscard]]
		//bool Intersect(const Vec2& a, const Bezier3& b);

		[[nodiscard]]
		constexpr bool Intersect(const Vec2& a, const Rect& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersect(const Vec2& a, const RectF& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersect(const Vec2& a, const Circle& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersect(const Vec2& a, const Ellipse& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersect(const Vec2& a, const Triangle& b) noexcept;

		[[nodiscard]]
		constexpr bool Intersect(const Vec2& a, const Quad& b) noexcept;

		[[nodiscard]]
		bool Intersect(const Vec2& a, const RoundRect& b) noexcept;

		[[nodiscard]]
		bool Intersect(const Vec2& a, const Polygon& b) noexcept;

		[[nodiscard]]
		bool Intersect(const Vec2& a, const MultiPolygon& b) noexcept;

		[[nodiscard]]
		bool Intersect(const Vec2& a, const LineString& b) noexcept;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type>::Vector2D(const value_type _x, const value_type _y) noexcept
		: x{ _x }
		, y{ _y } {}

	template <class Type>
	constexpr Vector2D<Type>::Vector2D(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ static_cast<value_type>(_y) } {}

	template <class Type>
	constexpr Vector2D<Type>::Vector2D(const Point p) noexcept
		: x{ static_cast<value_type>(p.x) }
		, y{ static_cast<value_type>(p.y) } {}

	template <class Type>
	template <class U>
	constexpr Vector2D<Type>::Vector2D(const Vector2D<U>& v) noexcept
		: x{ static_cast<value_type>(v.x) }
		, y{ static_cast<value_type>(v.y) } {}

	////////////////////////////////////////////////////////////////
	//
	//	elem
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::elem(const size_t index) const noexcept
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

	template <class Type>
	constexpr typename Vector2D<Type>::value_type* Vector2D<Type>::getPointer() noexcept
	{
		return &x;
	}

	template <class Type>
	constexpr const typename Vector2D<Type>::value_type* Vector2D<Type>::getPointer() const noexcept
	{
		return &x;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::operator +() const noexcept
	{
		return *this;
	}
	
	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::operator +(const Vector2D v) const noexcept
	{
		return{ (x + v.x), (y + v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::operator -() const noexcept
	{
		return{ -x, -y };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::operator -(const Vector2D v) const noexcept
	{
		return{ (x - v.x), (y - v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::operator *(const value_type s) const noexcept
	{
		return{ (x * s), (y * s) };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::operator *(const Vector2D v) const noexcept
	{
		return{ (x * v.x), (y * v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////
	
	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::operator /(const value_type s) const noexcept
	{
		const auto invS = (static_cast<value_type>(1.0) / s);

		return{ (x * invS), (y * invS) };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::operator /(const Vector2D v) const noexcept
	{
		return{ (x / v.x), (y / v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::operator +=(const Vector2D v) noexcept
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////
	
	template <class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::operator -=(const Vector2D v) noexcept
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::operator *=(const value_type s) noexcept
	{
		x *= s;
		y *= s;
		return *this;
	}

	template <class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::operator *=(const Vector2D v) noexcept
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::operator /=(const value_type s) noexcept
	{
		return (*this *= (static_cast<value_type>(1.0) / s));
	}

	template <class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::operator /=(const Vector2D v) noexcept
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	epsilonEquals
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr bool Vector2D<Type>::epsilonEquals(const Vector2D other, const value_type epsilon) const noexcept
	{
		return ((AbsDiff(x, other.x) <= epsilon)
			 && (AbsDiff(y, other.y) <= epsilon));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasSameDirection
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr bool Vector2D<Type>::hasSameDirection(const Vector2D other) const noexcept
	{
		return (0 < dot(other));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasOppositeDirection
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr bool Vector2D<Type>::hasOppositeDirection(const Vector2D other) const noexcept
	{
		return (dot(other) < 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isZero
	//
	////////////////////////////////////////////////////////////////
	
	template <class Type>
	constexpr bool Vector2D<Type>::isZero() const noexcept
	{
		return ((x == 0)
			 && (y == 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNaN
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	bool Vector2D<Type>::hasNaN() const noexcept
	{
		return (std::isnan(x)
			 || std::isnan(y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	minComponent, maxComponent
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::minComponent() const noexcept
	{
		return Min(x, y);
	}

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::maxComponent() const noexcept
	{
		return Max(x, y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr void Vector2D<Type>::clear() noexcept
	{
		x = 0;
		y = 0;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withX, withY
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::withX(const value_type _x) const noexcept
	{
		return{ _x, y };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::withY(const value_type _y) const noexcept
	{
		return{ x, _y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::set(const value_type _x, const value_type _y) noexcept
	{
		x = _x;
		y = _y;
		return *this;
	}

	template <class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::set(const Vector2D v) noexcept
	{
		return (*this = v);
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::movedBy(const value_type _x, const value_type _y) const noexcept
	{
		return{ (x + _x), (y + _y) };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::movedBy(const Vector2D v) const noexcept
	{
		return{ (x + v.x), (y + v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::moveBy(const value_type _x, const value_type _y) noexcept
	{
		x += _x;
		y += _y;
		return *this;
	}

	template <class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::moveBy(const Vector2D v) noexcept
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	dot
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	template <class U>
	constexpr auto Vector2D<Type>::dot(const Vector2D<U> v) const noexcept
	{
		return ((x * v.x) + (y * v.y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	cross
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	template <class U>
	constexpr auto Vector2D<Type>::cross(const Vector2D<U> v) const noexcept
	{
		return ((x * v.y) - (y * v.x));
	}

	////////////////////////////////////////////////////////////////
	//
	//	horizontalAspectRatio
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::horizontalAspectRatio() const noexcept
	{
		if (y == 0)
		{
			return 0;
		}

		return (x / y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	length
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	typename Vector2D<Type>::value_type Vector2D<Type>::length() const noexcept
	{
		return std::sqrt((x * x) + (y * y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	lengthSq
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::lengthSq() const noexcept
	{
		return ((x * x) + (y * y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	invLength
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	typename Vector2D<Type>::value_type Vector2D<Type>::invLength() const noexcept
	{
		return (static_cast<value_type>(1.0) / length());
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanLength
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::manhattanLength() const noexcept
	{
		return (Abs(x) + Abs(y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceFrom
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::manhattanDistanceFrom(const value_type _x, const value_type _y) const noexcept
	{
		const auto xMinMax = MinMax(x, _x);
		const auto yMinMax = MinMax(y, _y);
		return ((xMinMax.second - xMinMax.first)
			  + (yMinMax.second - yMinMax.first));
	}

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::manhattanDistanceFrom(const Vector2D v) const noexcept
	{
		return manhattanDistanceFrom(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceTo
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::manhattanDistanceTo(const value_type _x, const value_type _y) const noexcept
	{
		return manhattanDistanceFrom(_x, _y);
	}

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::manhattanDistanceTo(const Vector2D v) const noexcept
	{
		return manhattanDistanceFrom(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFrom
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	typename Vector2D<Type>::value_type Vector2D<Type>::distanceFrom(const value_type _x, const value_type _y) const noexcept
	{
		return std::sqrt(distanceFromSq(_x, _y));
	}

	template <class Type>
	typename Vector2D<Type>::value_type Vector2D<Type>::distanceFrom(const Vector2D v) const noexcept
	{
		return std::sqrt(distanceFromSq(v.x, v.y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFromSq
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::distanceFromSq(const value_type _x, const value_type _y) const noexcept
	{
		const value_type xx = (x - _x) * (x - _x);
		const value_type yy = (y - _y) * (y - _y);
		return (xx + yy);
	}

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::distanceFromSq(const Vector2D v) const noexcept
	{
		return distanceFromSq(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceTo
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	typename Vector2D<Type>::value_type Vector2D<Type>::distanceTo(const value_type _x, const value_type _y) const noexcept
	{
		return distanceFrom(_x, _y);
	}

	template <class Type>
	typename Vector2D<Type>::value_type Vector2D<Type>::distanceTo(const Vector2D v) const noexcept
	{
		return distanceFrom(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceToSq
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::distanceToSq(const value_type _x, const value_type _y) const noexcept
	{
		return distanceFromSq(_x, _y);
	}

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::distanceToSq(const Vector2D v) const noexcept
	{
		return distanceFromSq(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withLength
	//
	////////////////////////////////////////////////////////////////
	
	template <class Type>
	Vector2D<Type> Vector2D<Type>::withLength(const value_type _length) const noexcept
	{
		if (const value_type len = length())
		{
			return (*this * (_length / len));
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setLength
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Vector2D<Type>& Vector2D<Type>::setLength(const value_type _length) noexcept
	{
		if (const value_type len = length())
		{
			*this *= (_length / len);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withMaxLength
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Vector2D<Type> Vector2D<Type>::withMaxLength(const value_type maxLength) const noexcept
	{
		if (const value_type len = length();
			maxLength < len)
		{
			return (*this * (maxLength / len));
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setMaxLength
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Vector2D<Type>& Vector2D<Type>::setMaxLength(const value_type maxLength) noexcept
	{
		if (const value_type len = length();
			maxLength < len)
		{
			*this *= (maxLength / len);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalized
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Vector2D<Type> Vector2D<Type>::normalized() const noexcept
	{
		const value_type lenSq = lengthSq();

		if (lenSq == 0)
		{
			return *this;
		}

		const value_type invLen = (static_cast<value_type>(1.0) / std::sqrt(lenSq));
		return{ (x * invLen), (y * invLen) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalized_or
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Vector2D<Type> Vector2D<Type>::normalized_or(const Vector2D valueIfZero) const noexcept
	{
		const value_type lenSq = lengthSq();

		if (lenSq == 0)
		{
			return valueIfZero;
		}

		const value_type invLen = (static_cast<value_type>(1.0) / std::sqrt(lenSq));
		return{ (x * invLen), (y * invLen) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	normalize
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Vector2D<Type>& Vector2D<Type>::normalize() noexcept
	{
		const value_type lenSq = lengthSq();

		if (lenSq == 0)
		{
			x = y = 0;
		}
		else
		{
			const value_type invLen = (static_cast<value_type>(1.0) / std::sqrt(lenSq));
			x *= invLen;
			y *= invLen;
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated, rotate
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Vector2D<Type> Vector2D<Type>::rotated(const value_type angle) const noexcept
	{
		const value_type s = std::sin(angle);
		const value_type c = std::cos(angle);
		return{ (x * c - y * s), (x * s + y * c) };
	}

	template <class Type>
	Vector2D<Type>& Vector2D<Type>::rotate(const value_type angle) noexcept
	{
		return (*this = rotated(angle));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAt, rotateAt
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Vector2D<Type> Vector2D<Type>::rotatedAt(const Vector2D center, const value_type angle) const noexcept
	{
		const value_type s = std::sin(angle);
		const value_type c = std::cos(angle);
		const Vector2D b = (*this - center);
		const Vector2D p{ (b.x * c - b.y * s), (b.x * s + b.y * c) };
		return (p + center);
	}

	template <class Type>
	Vector2D<Type>& Vector2D<Type>::rotateAt(const Vector2D center, const value_type angle) noexcept
	{
		return (*this = rotatedAt(center, angle));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated90, rotate90
	//
	////////////////////////////////////////////////////////////////

	template<class Type>
	constexpr Vector2D<Type> Vector2D<Type>::rotated90(const int32 n) const noexcept
	{
		switch (n % 4) // 時計回りに何回 90° 回転するか
		{
		case 1:
		case -3:
			return { -y,x }; // 1 回または -3 回
		case 2:
		case -2:
			return { -x,-y }; // 2 回または -2 回
		case 3:
		case -1:
			return { y,-x }; // 3 回または -1 回
		default:
			return *this; // 0 回
		}
	}

	template<class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::rotate90(const int32 n) noexcept
	{
		return (*this = rotated90(n));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated90At, rotate90At
	//
	////////////////////////////////////////////////////////////////

	template<class Type>
	constexpr Vector2D<Type> Vector2D<Type>::rotated90At(const Vector2D center, const int32 n) const noexcept
	{
		return ((*this - center).rotated90(n) + center);
	}

	template<class Type>
	constexpr Vector2D<Type>& Vector2D<Type>::rotate90At(const Vector2D center, const int32 n) noexcept
	{
		return (*this = rotated90At(center, n));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAngle
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	typename Vector2D<Type>::value_type Vector2D<Type>::getAngle() const noexcept
	{
		if (isZero())
		{
			return 0;
		}

		return std::atan2(x, -y);
	}

	template <class Type>
	typename Vector2D<Type>::value_type Vector2D<Type>::getAngle(const Vector2D other) const noexcept
	{
		if (isZero() || other.isZero())
		{
			return Math::NaN_v<Type>;
		}

		return std::atan2(cross(other), dot(other));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPerpendicularCW, getPerpendicularCCW
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::getPerpendicularCW() const noexcept
	{
		return{ -y, x };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::getPerpendicularCCW() const noexcept
	{
		return{ y, -x };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMidpoint
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::getMidpoint(const Vector2D other) const noexcept
	{
		constexpr value_type half(0.5);
		return{ (x * half + other.x * half), (y * half + other.y * half) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	projectOnVector
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::projectOnVector(const Vector2D v) const noexcept
	{
		if (const value_type ontoLengthSq = v.lengthSq())
		{
			return (v * dot(v) / ontoLengthSq);
		}
		else
		{
			return{ 0, 0 };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPointByAngleAndDistance
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Vector2D<Type> Vector2D<Type>::getPointByAngleAndDistance(const value_type angle, const value_type distance) const noexcept
	{
		const auto s = std::sin(angle);
		const auto c = std::cos(angle);
		return{ ((s * distance) + x), ((-c * distance) + y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	lerp
	//
	////////////////////////////////////////////////////////////////
	
	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::lerp(const Vector2D other, const value_type f) const noexcept
	{
		return{ (x + (other.x - x) * f), (y + (other.y - y) * f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	asPoint
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Point Vector2D<Type>::asPoint() const noexcept
	{
		return{ static_cast<Point::value_type>(x), static_cast<Point::value_type>(y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	template <class Shape2DType>
	constexpr bool Vector2D<Type>::intersects(const Shape2DType& other) const
	{
		if constexpr (std::is_same_v<value_type, double>)
		{
			return Geometry2D::Intersect(*this, other);
		}
		else
		{
			return Geometry2D::Intersect(Vector2D<double>{ *this }, other);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	uint64 Vector2D<Type>::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	xx, xy, yx, yy, x0, y0
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::xx() const noexcept
	{
		return{ x, x };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::xy() const noexcept
	{
		return *this;
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::yx() const noexcept
	{
		return{ y, x };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::yy() const noexcept
	{
		return{ y, y };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::x0() const noexcept
	{
		return{ x, 0 };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::y0() const noexcept
	{
		return{ y, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xy0
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector3D<Type> Vector2D<Type>::xy0() const noexcept
	{
		return{ x, y, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xy00
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector4D<Type> Vector2D<Type>::xy00() const noexcept
	{
		return{ x, y, 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::Zero() noexcept
	{
		return{ 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	One
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::One() noexcept
	{
		return{ 1, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	All
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::All(const value_type value) noexcept
	{
		return{ value, value };
	}

	////////////////////////////////////////////////////////////////
	//
	//	UnitX, UnitY
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::UnitX() noexcept
	{
		return{ 1, 0 };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::UnitY() noexcept
	{
		return{ 0, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Left, Right, Up, Down
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::Left(const value_type length) noexcept
	{
		return{ -length, 0 };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::Right(const value_type length) noexcept
	{
		return{ length, 0 };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::Up(const value_type length) noexcept
	{
		return{ 0, -length };
	}

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::Down(const value_type length) noexcept
	{
		return{ 0, length };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorCenter
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::AnchorCenter() noexcept
	{
		return{ Type(0.5), Type(0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorTopLeft
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::AnchorTopLeft() noexcept
	{
		return{ 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorTopCenter
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::AnchorTopCenter() noexcept
	{
		return{ Type(0.5), 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorTopRight
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::AnchorTopRight() noexcept
	{
		return{ 1, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorRightCenter
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::AnchorRightCenter() noexcept
	{
		return{ 1, Type(0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorBottomRight
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::AnchorBottomRight() noexcept
	{
		return{ 1, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorBottomCenter
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::AnchorBottomCenter() noexcept
	{
		return{ Type(0.5), 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorBottomLeft
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::AnchorBottomLeft() noexcept
	{
		return{ 0, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorLeftCenter
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr Vector2D<Type> Vector2D<Type>::AnchorLeftCenter() noexcept
	{
		return{ 0, Type(0.5) };
	}
}

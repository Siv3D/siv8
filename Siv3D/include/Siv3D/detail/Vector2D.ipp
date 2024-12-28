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

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>::Vector2D(const value_type _x, const value_type _y) noexcept
		: x{ _x }
		, y{ _y } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>::Vector2D(const Concept::Arithmetic auto _x, const Concept::Arithmetic auto _y) noexcept
		: x{ static_cast<value_type>(_x) }
		, y{ static_cast<value_type>(_y) } {}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>::Vector2D(const Point p) noexcept
		: x{ static_cast<value_type>(p.x) }
		, y{ static_cast<value_type>(p.y) } {}

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	constexpr Vector2D<Float>::Vector2D(const Vector2D<U>& v) noexcept
		: x{ static_cast<value_type>(v.x) }
		, y{ static_cast<value_type>(v.y) } {}

	////////////////////////////////////////////////////////////////
	//
	//	elem
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::elem(const size_t index) const noexcept
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

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type* Vector2D<Float>::getPointer() noexcept
	{
		return &x;
	}

	template <Concept::FloatingPoint Float>
	constexpr const typename Vector2D<Float>::value_type* Vector2D<Float>::getPointer() const noexcept
	{
		return &x;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::operator +() const noexcept
	{
		return *this;
	}
	
	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::operator +(const Vector2D v) const noexcept
	{
		return{ (x + v.x), (y + v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::operator -() const noexcept
	{
		return{ -x, -y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::operator -(const Vector2D v) const noexcept
	{
		return{ (x - v.x), (y - v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::operator *(const value_type s) const noexcept
	{
		return{ (x * s), (y * s) };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::operator *(const Vector2D v) const noexcept
	{
		return{ (x * v.x), (y * v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////
	
	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::operator /(const value_type s) const noexcept
	{
		const auto invS = (static_cast<value_type>(1.0) / s);

		return{ (x * invS), (y * invS) };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::operator /(const Vector2D v) const noexcept
	{
		return{ (x / v.x), (y / v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::operator +=(const Vector2D v) noexcept
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
	
	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::operator -=(const Vector2D v) noexcept
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

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::operator *=(const value_type s) noexcept
	{
		x *= s;
		y *= s;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::operator *=(const Vector2D v) noexcept
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

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::operator /=(const value_type s) noexcept
	{
		return (*this *= (static_cast<value_type>(1.0) / s));
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::operator /=(const Vector2D v) noexcept
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

	template <Concept::FloatingPoint Float>
	constexpr bool Vector2D<Float>::epsilonEquals(const Vector2D other, const value_type epsilon) const noexcept
	{
		return ((AbsDiff(x, other.x) <= epsilon)
			 && (AbsDiff(y, other.y) <= epsilon));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasSameDirection
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr bool Vector2D<Float>::hasSameDirection(const Vector2D other) const noexcept
	{
		return (0 < dot(other));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasOppositeDirection
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr bool Vector2D<Float>::hasOppositeDirection(const Vector2D other) const noexcept
	{
		return (dot(other) < 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isZero
	//
	////////////////////////////////////////////////////////////////
	
	template <Concept::FloatingPoint Float>
	constexpr bool Vector2D<Float>::isZero() const noexcept
	{
		return ((x == 0)
			 && (y == 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasNaN
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	bool Vector2D<Float>::hasNaN() const noexcept
	{
		return (std::isnan(x)
			 || std::isnan(y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	minComponent, maxComponent
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::minComponent() const noexcept
	{
		return Min(x, y);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::maxComponent() const noexcept
	{
		return Max(x, y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr void Vector2D<Float>::clear() noexcept
	{
		x = 0;
		y = 0;
	}

	////////////////////////////////////////////////////////////////
	//
	//	withX, withY
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::withX(const value_type _x) const noexcept
	{
		return{ _x, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::withY(const value_type _y) const noexcept
	{
		return{ x, _y };
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::set(const value_type _x, const value_type _y) noexcept
	{
		x = _x;
		y = _y;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::set(const Vector2D v) noexcept
	{
		return (*this = v);
	}

	////////////////////////////////////////////////////////////////
	//
	//	movedBy
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::movedBy(const value_type _x, const value_type _y) const noexcept
	{
		return{ (x + _x), (y + _y) };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::movedBy(const Vector2D v) const noexcept
	{
		return{ (x + v.x), (y + v.y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::moveBy(const value_type _x, const value_type _y) noexcept
	{
		x += _x;
		y += _y;
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::moveBy(const Vector2D v) noexcept
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

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	constexpr auto Vector2D<Float>::dot(const Vector2D<U> v) const noexcept
	{
		return ((x * v.x) + (y * v.y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	cross
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	template <Concept::FloatingPoint U>
	constexpr auto Vector2D<Float>::cross(const Vector2D<U> v) const noexcept
	{
		return ((x * v.y) - (y * v.x));
	}

	////////////////////////////////////////////////////////////////
	//
	//	horizontalAspectRatio
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::horizontalAspectRatio() const noexcept
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

	template <Concept::FloatingPoint Float>
	typename Vector2D<Float>::value_type Vector2D<Float>::length() const noexcept
	{
		return std::sqrt((x * x) + (y * y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	lengthSq
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::lengthSq() const noexcept
	{
		return ((x * x) + (y * y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	invLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector2D<Float>::value_type Vector2D<Float>::invLength() const noexcept
	{
		return (static_cast<value_type>(1.0) / length());
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanLength
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::manhattanLength() const noexcept
	{
		return (Abs(x) + Abs(y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceFrom
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::manhattanDistanceFrom(const value_type _x, const value_type _y) const noexcept
	{
		const auto xMinMax = MinMax(x, _x);
		const auto yMinMax = MinMax(y, _y);
		return ((xMinMax.second - xMinMax.first)
			  + (yMinMax.second - yMinMax.first));
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::manhattanDistanceFrom(const Vector2D v) const noexcept
	{
		return manhattanDistanceFrom(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	manhattanDistanceTo
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::manhattanDistanceTo(const value_type _x, const value_type _y) const noexcept
	{
		return manhattanDistanceFrom(_x, _y);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::manhattanDistanceTo(const Vector2D v) const noexcept
	{
		return manhattanDistanceFrom(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFrom
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector2D<Float>::value_type Vector2D<Float>::distanceFrom(const value_type _x, const value_type _y) const noexcept
	{
		return std::sqrt(distanceFromSq(_x, _y));
	}

	template <Concept::FloatingPoint Float>
	typename Vector2D<Float>::value_type Vector2D<Float>::distanceFrom(const Vector2D v) const noexcept
	{
		return std::sqrt(distanceFromSq(v.x, v.y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceFromSq
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::distanceFromSq(const value_type _x, const value_type _y) const noexcept
	{
		const value_type xx = (x - _x) * (x - _x);
		const value_type yy = (y - _y) * (y - _y);
		return (xx + yy);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::distanceFromSq(const Vector2D v) const noexcept
	{
		return distanceFromSq(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceTo
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector2D<Float>::value_type Vector2D<Float>::distanceTo(const value_type _x, const value_type _y) const noexcept
	{
		return distanceFrom(_x, _y);
	}

	template <Concept::FloatingPoint Float>
	typename Vector2D<Float>::value_type Vector2D<Float>::distanceTo(const Vector2D v) const noexcept
	{
		return distanceFrom(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	distanceToSq
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::distanceToSq(const value_type _x, const value_type _y) const noexcept
	{
		return distanceFromSq(_x, _y);
	}

	template <Concept::FloatingPoint Float>
	constexpr typename Vector2D<Float>::value_type Vector2D<Float>::distanceToSq(const Vector2D v) const noexcept
	{
		return distanceFromSq(v.x, v.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	withLength
	//
	////////////////////////////////////////////////////////////////
	
	template <Concept::FloatingPoint Float>
	Vector2D<Float> Vector2D<Float>::withLength(const value_type _length) const noexcept
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

	template <Concept::FloatingPoint Float>
	Vector2D<Float>& Vector2D<Float>::setLength(const value_type _length) noexcept
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

	template <Concept::FloatingPoint Float>
	Vector2D<Float> Vector2D<Float>::withMaxLength(const value_type maxLength) const noexcept
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

	template <Concept::FloatingPoint Float>
	Vector2D<Float>& Vector2D<Float>::setMaxLength(const value_type maxLength) noexcept
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

	template <Concept::FloatingPoint Float>
	Vector2D<Float> Vector2D<Float>::normalized() const noexcept
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

	template <Concept::FloatingPoint Float>
	Vector2D<Float> Vector2D<Float>::normalized_or(const Vector2D valueIfZero) const noexcept
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

	template <Concept::FloatingPoint Float>
	Vector2D<Float>& Vector2D<Float>::normalize() noexcept
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

	template <Concept::FloatingPoint Float>
	Vector2D<Float> Vector2D<Float>::rotated(const value_type angle) const noexcept
	{
		const value_type s = std::sin(angle);
		const value_type c = std::cos(angle);
		return{ (x * c - y * s), (x * s + y * c) };
	}

	template <Concept::FloatingPoint Float>
	Vector2D<Float>& Vector2D<Float>::rotate(const value_type angle) noexcept
	{
		return (*this = rotated(angle));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAt, rotateAt
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	Vector2D<Float> Vector2D<Float>::rotatedAt(const Vector2D center, const value_type angle) const noexcept
	{
		const value_type s = std::sin(angle);
		const value_type c = std::cos(angle);
		const Vector2D b = (*this - center);
		const Vector2D p{ (b.x * c - b.y * s), (b.x * s + b.y * c) };
		return (p + center);
	}

	template <Concept::FloatingPoint Float>
	Vector2D<Float>& Vector2D<Float>::rotateAt(const Vector2D center, const value_type angle) noexcept
	{
		return (*this = rotatedAt(center, angle));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated90, rotate90
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::rotated90(const int32 n) const noexcept
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

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::rotate90(const int32 n) noexcept
	{
		return (*this = rotated90(n));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated90At, rotate90At
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::rotated90At(const Vector2D center, const int32 n) const noexcept
	{
		return ((*this - center).rotated90(n) + center);
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float>& Vector2D<Float>::rotate90At(const Vector2D center, const int32 n) noexcept
	{
		return (*this = rotated90At(center, n));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAngle
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	typename Vector2D<Float>::value_type Vector2D<Float>::getAngle() const noexcept
	{
		if (isZero())
		{
			return 0;
		}

		return std::atan2(x, -y);
	}

	template <Concept::FloatingPoint Float>
	typename Vector2D<Float>::value_type Vector2D<Float>::getAngle(const Vector2D other) const noexcept
	{
		if (isZero() || other.isZero())
		{
			return Math::NaN_v<Float>;
		}

		return std::atan2(cross(other), dot(other));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getPerpendicularCW, getPerpendicularCCW
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::getPerpendicularCW() const noexcept
	{
		return{ -y, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::getPerpendicularCCW() const noexcept
	{
		return{ y, -x };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMidpoint
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::getMidpoint(const Vector2D other) const noexcept
	{
		constexpr value_type half(0.5);
		return{ (x * half + other.x * half), (y * half + other.y * half) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	projectOnVector
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::projectOnVector(const Vector2D v) const noexcept
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

	template <Concept::FloatingPoint Float>
	Vector2D<Float> Vector2D<Float>::getPointByAngleAndDistance(const value_type angle, const value_type distance) const noexcept
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
	
	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::lerp(const Vector2D other, const value_type f) const noexcept
	{
		return{ (x + (other.x - x) * f), (y + (other.y - y) * f) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	asPoint
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Point Vector2D<Float>::asPoint() const noexcept
	{
		return{ static_cast<Point::value_type>(x), static_cast<Point::value_type>(y) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	template <class Shape2DType>
	constexpr bool Vector2D<Float>::intersects(const Shape2DType& other) const
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

	template <Concept::FloatingPoint Float>
	uint64 Vector2D<Float>::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	xx, xy, yx, yy, x0, y0
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::xx() const noexcept
	{
		return{ x, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::xy() const noexcept
	{
		return *this;
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::yx() const noexcept
	{
		return{ y, x };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::yy() const noexcept
	{
		return{ y, y };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::x0() const noexcept
	{
		return{ x, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::y0() const noexcept
	{
		return{ y, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xy0
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector3D<Float> Vector2D<Float>::xy0() const noexcept
	{
		return{ x, y, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	xy00
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector4D<Float> Vector2D<Float>::xy00() const noexcept
	{
		return{ x, y, 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Zero
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::Zero() noexcept
	{
		return{ 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	One
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::One() noexcept
	{
		return{ 1, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	All
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::All(const value_type value) noexcept
	{
		return{ value, value };
	}

	////////////////////////////////////////////////////////////////
	//
	//	UnitX, UnitY
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::UnitX() noexcept
	{
		return{ 1, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::UnitY() noexcept
	{
		return{ 0, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	Left, Right, Up, Down
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::Left(const value_type length) noexcept
	{
		return{ -length, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::Right(const value_type length) noexcept
	{
		return{ length, 0 };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::Up(const value_type length) noexcept
	{
		return{ 0, -length };
	}

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::Down(const value_type length) noexcept
	{
		return{ 0, length };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorCenter
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::AnchorCenter() noexcept
	{
		return{ static_cast<value_type>(0.5), static_cast<value_type>(0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorTopLeft
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::AnchorTopLeft() noexcept
	{
		return{ 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorTopCenter
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::AnchorTopCenter() noexcept
	{
		return{ static_cast<value_type>(0.5), 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorTopRight
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::AnchorTopRight() noexcept
	{
		return{ 1, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorRightCenter
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::AnchorRightCenter() noexcept
	{
		return{ 1, static_cast<value_type>(0.5) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorBottomRight
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::AnchorBottomRight() noexcept
	{
		return{ 1, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorBottomCenter
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::AnchorBottomCenter() noexcept
	{
		return{ static_cast<value_type>(0.5), 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorBottomLeft
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::AnchorBottomLeft() noexcept
	{
		return{ 0, 1 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	AnchorLeftCenter
	//
	////////////////////////////////////////////////////////////////

	template <Concept::FloatingPoint Float>
	constexpr Vector2D<Float> Vector2D<Float>::AnchorLeftCenter() noexcept
	{
		return{ 0, static_cast<value_type>(0.5) };
	}
}

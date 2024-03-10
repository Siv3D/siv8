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
	//	dot
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::dot(const Vector2D v) const noexcept
	{
		return ((x * v.x) + (y * v.y));
	}

	////////////////////////////////////////////////////////////////
	//
	//	cross
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	constexpr typename Vector2D<Type>::value_type Vector2D<Type>::cross(const Vector2D v) const noexcept
	{
		return ((x * v.y) - (y * v.x));
	}

}

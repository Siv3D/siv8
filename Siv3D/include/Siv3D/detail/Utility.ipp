//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	template <Concept::Scalar Scalar>
	constexpr Scalar Min(const Scalar a, const Scalar b) noexcept
	{
		return ((b < a) ? b : a);
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr const Type& Min(const Type& a, const Type& b) noexcept(noexcept(b < a))
	{
		return ((b < a) ? b : a);
	}

	template <Concept::Scalar Scalar>
	constexpr Scalar Max(const Scalar a, const Scalar b) noexcept
	{
		return ((a < b) ? b : a);
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr const Type& Max(const Type& a, const Type& b) noexcept(noexcept(a < b))
	{
		return ((a < b) ? b : a);
	}

	template <Concept::Scalar Scalar>
	constexpr Scalar Clamp(const Scalar v, const Scalar min, const Scalar max) noexcept
	{
		if (max < v)
		{
			return max;
		}

		if (v < min)
		{
			return min;
		}

		return v;
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr const Type& Clamp(const Type& v, const Type& min, const Type& max) noexcept(noexcept(max < v))
	{
		if (max < v)
		{
			return max;
		}

		if (v < min)
		{
			return min;
		}

		return v;
	}

	template <Concept::Scalar Scalar>
	constexpr bool InRange(const Scalar v, const Scalar min, const Scalar max) noexcept
	{
		return ((min <= v) && (v <= max));
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr bool InRange(const Type& v, const Type& min, const Type& max) noexcept(noexcept(v < min))
	{
		return ((min <= v) && (v <= max));
	}

	template <Concept::Scalar Scalar>
	constexpr bool InOpenRange(const Scalar v, const Scalar min, const Scalar max) noexcept
	{
		return ((min < v) && (v < max));
	}

	template <class Type> requires (not Concept::Scalar<Type>)
	constexpr bool InOpenRange(const Type& v, const Type& min, const Type& max) noexcept(noexcept(v < min))
	{
		return ((min < v) && (v < max));
	}
}
